#include <eosio/eosio.hpp>
#include <eosio/print.hpp>
#include <eosio/asset.hpp>
#include <eosio/system.hpp>
#include <vector>
//using randomnese
#include <provable/eos_api.hpp>

using namespace eosio;

// The contract
CONTRACT tictactoe123 : public contract {
	public:

	using contract::contract;

	tictactoe123(name receiver, name code, datastream<const char *> ds) : contract(receiver, code, ds),
		hodl_symbol("EOS", 4) {} //singleton initialize

	TABLE game_record {
		name host;
		name opponent;
		asset hoststake;
		asset opponentstake;
		
		std::vector<std::uint8_t> board{0,0,0,0,0,0,0,0,0};
		name turn;
		name winner;
		checksum256 provablequeryId;
	
		bool is_empty_cell(uint16_t row, uint16_t col) {return (board[row*3+col]==0);}
		bool is_valid_movement(name by, uint16_t row, uint16_t col) {
			if (winner==name() && by==turn && is_empty_cell(row, col) ) {
				uint8_t set=1;
				if (by==host) {
					board[row*3+col]=set;
					turn=opponent; //next turn
				}
				else {
					set=2;
					board[row*3+col]=set;
					turn=host; //next turn
				}
			
				uint8_t test=((board[row*3+0]&board[row*3+1]&board[row*3+2]) 
					|(board[0*3+col]&board[1*3+col]&board[2*3+col])
					|(board[0]&board[4]&board[8])
					|(board[2]&board[4]&board[6]));
			
				if (test==set)
					winner=by;

				return true;
			}
			return false;
		}

		uint64_t primary_key() const {return host.value;}
		uint128_t secondary_key() const {return combine_ids(host.value,opponent.value);}
		checksum256 third_key() const {return provablequeryId;}
	};
	
	typedef eosio::multi_index<name("games"), game_record,
		eosio::indexed_by<name("gameskey"), eosio::const_mem_fun<game_record, uint128_t, &game_record::secondary_key>>,
		eosio::indexed_by<name("gametkey"), eosio::const_mem_fun<game_record, checksum256, &game_record::third_key>>
	> game_index;

	ACTION welcome(name host, name opponent) {
		//action can only be taken by contract account
		require_auth(get_self());
		print("Welcome, ", host, " and ", opponent,"!");
	}

	[[eosio::on_notify("eosio.token::transfer")]]
	void create(name from, name to, eosio::asset quantity, std::string memo) {
		if (from == get_self() || to != get_self())
			return;

		check(quantity.amount > 0, "When pigs fly");
		check(quantity.symbol == hodl_symbol, "These are not the droids you are looking for.");
 
		name opponent = name(memo);
	
		check(is_account(opponent),"opponent account not found");
		check(from!=opponent, "Could not challenge youself!");

		game_index _game(get_self(), get_self().value);
		//find using secondary key
		auto _gameskey = _game.get_index<name("gameskey")>(); //set secondary key
		auto itrh = _gameskey.find(combine_ids(from.value, opponent.value));
		auto itrc = _gameskey.find(combine_ids(opponent.value, from.value));
		if (itrh==_gameskey.end() && itrc==_gameskey.end()) {
			//ram charge to contract
			_game.emplace(get_self(), [&](auto& game) { 
				game.host = from;
				game.opponent = opponent;
				game.hoststake = quantity;
				game.opponentstake = asset(0.0000,hodl_symbol);
				//**game first turn will select randomly
				game.turn = from;
			});
		}
		else if (itrh!=_gameskey.end()) {
			//ram charge to same_payer
			_game.modify(*itrh, same_payer, [&](auto& game) { 
				game.hoststake += quantity; //add more stake
			});
		}
		else {
			//ram charge to same_payer		
			_game.modify(*itrc, same_payer, [&](auto& game) {
				//on opponent accept the game (determine first turn randomly)
				if (game.opponentstake.amount==0) {
					game.provablequeryId = execprovablequeryWolframAlpha12(); //*save queryId
				}
				game.opponentstake += quantity;				
			});
		}
	}
	
	ACTION move(name opponent, name host, name by, uint16_t row, uint16_t col) {
		check(has_auth(by), "please auth before!");
		check(by==opponent || by==host, "only opponent or host can move!");
		check(row<3 && col < 3, "invalid row or col!");
		game_index _game(get_self(), get_self().value);
		auto _gameskey = _game.get_index<name("gameskey")>();
		auto itr = _gameskey.find(combine_ids(host.value, opponent.value));
		check(itr!=_gameskey.end(), "game not found.");
		check(itr->winner==name(), "game over!");
		//can move when the stake balanced
		check(itr->hoststake==itr->opponentstake, "Stake not balance");
		_game.modify(*itr, same_payer, [&]( auto& game ) {
			check(game.is_valid_movement(by, row, col), "invalid move.");
			//on winning game
			if (game.winner!=name())
				game.provablequeryId = execprovablequeryEOSvsUSD(); //will info EOSvsUSD and pay to winner
			//on tie game
			else if (is_draw(itr->board)) {
				payback(itr->host, itr->hoststake, "Stake refund.");
				payback(itr->opponent, itr->opponentstake, "Stake refund.");
			}
			//next turn
			else {
				game.provablequeryId = execprovablequeryTimeKeeper(); //time keeper restart
			}
		});
	}
	
	//callback from execprovablequeryWolframAlpha12 or execprovablequeryEOSvsUSD
	ACTION callback(
		const eosio::checksum256 queryId,
		const std::vector<uint8_t> result,
		const std::vector<uint8_t> proof)
	{
		require_auth(provable_cbAddress());
		const std::string result_str = vector_to_string(result);
		print(" Result: ", result_str);
		print(" Proof length: ", proof.size());
		game_index _game(get_self(), get_self().value);
		auto _gametkey = _game.get_index<name("gametkey")>();
		auto itr = _gametkey.find(queryId); //*find game base on queryId
		if (itr!=_gametkey.end()) {
			//feedback on winning
			if (itr->winner!=name())
				//pay to winner
				payback(itr->winner, itr->hoststake+itr->opponentstake, "You got the prize. EOS/USD=" + result_str);
			//feedback on determine game first turn randomly
			else if (itr->turn==name()) {
				_game.modify(*itr, same_payer, [&](auto& game) {
					game.turn= (result_str=="1"?game.host:game.opponent);
					game.provablequeryId = execprovablequeryTimeKeeper(); //time keeper start
				});
			}
			//turn time out, then determine winner and pay to winner
			else {				
				_game.modify(*itr, same_payer, [&](auto& game) {
					game.winner=(game.turn==game.host?game.opponent:game.host); //determine winner base on next turn
					game.provablequeryId = execprovablequeryEOSvsUSD(); //will info EOSvsUSD and pay to winner
				});
			}
		}
	}	

 	ACTION close(name opponent, name host) {
		//find by host (primary key)
		check(has_auth(host), "Please auth yourself.");
		game_index _game(get_self(), get_self().value);
		//find using secondary key
		auto _gameskey = _game.get_index<name("gameskey")>();
		auto itr = _gameskey.find(combine_ids(host.value, opponent.value));
		if (itr!=_gameskey.end()) {
			if (itr->winner!=name("") || is_draw(itr->board))
				_gameskey.erase(itr);
			else {
				check(itr->opponentstake==asset(0,hodl_symbol), "Close failed, opponent has staked."); 
				payback(itr->host, itr->hoststake, "Your stake freed.");
				_gameskey.erase(itr);
			}
		}
		else {
			check(false, "Game not found.");
		}
	}

	private: 
	
	const symbol hodl_symbol;

	// concatenation of ids example
	static uint128_t combine_ids(const uint64_t &x, const uint64_t &y) {
		return (uint128_t{x} << 64) | y;
	}
	
	bool is_draw(std::vector<std::uint8_t> board) {
		uint8_t pos=0;
		while (pos < 9) {
			if (board[pos]==0) break;
			pos++;			
		}
		return pos==9;
	}
	
	//return provable idQuery for determine game first turn randomly
	checksum256 execprovablequeryWolframAlpha12()
      	{
		print("Sending query to Provable...");
		return provable_query("WolframAlpha", "random number between 1 and 2"); //1=host, 2=opponent
	}

	//return provable idQuery for each turn time keeper (10 seconds)
	checksum256 execprovablequeryTimeKeeper()
      	{
		print("Sending query to Provable...");
		return provable_query(10, "WolframAlpha", "random number between 1 and 2"); //10 seconds
	}		
	
	//return provable idQuery for display current EOS vs USD price when game winning
	checksum256 execprovablequeryEOSvsUSD()
      	{
		print("Sending query to Provable...");
		return provable_query("URL", "json(https://min-api.cryptocompare.com/data/price?fsym=EOS&tsyms=USD).USD",\
			(proofType_Android | proofStorage_IPFS));
	}
	
	void payback(name to, asset quantity, std::string memo) {
		action{
			permission_level{get_self(), "active"_n},
			"eosio.token"_n,
			"transfer"_n,
			std::make_tuple(get_self(), to, quantity, memo)
		}.send();
	}
};
