#include <eosio/eosio.hpp>
#include <eosio/print.hpp>
#include <eosio/asset.hpp>
#include <eosio/system.hpp>
#include <vector>

using namespace eosio;

// The contract
CONTRACT tictactoe : public contract {
	public:

	using contract::contract;

	tictactoe(name receiver, name code, datastream<const char *> ds) : contract(receiver, code, ds),
		hodl_symbol("SYS", 4) {} //singleton initialize

	TABLE game_record {
		name host;
		name opponent;
		eosio::asset hoststake;
		eosio::asset opponentstake;
		
		std::vector<std::uint8_t> board{0,0,0,0,0,0,0,0,0};
		name turn;
		name winner;
	
		bool is_empty_cell(uint16_t row, uint16_t col) {return (board[row*3+col]==0);}
		bool is_valid_movement(name by, uint16_t row, uint16_t col) {
			if (winner==name() && by==turn && is_empty_cell(row, col) ) {
				uint8_t set=1;
				if (by==host) {
					board[row*3+col]=set;
					turn=opponent;
				}
				else {
					set=2;
					board[row*3+col]=set;
					turn=host;
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
	};
	
	typedef eosio::multi_index<name("games"), game_record,
		eosio::indexed_by<name("gameskey"), eosio::const_mem_fun<game_record, uint128_t, &game_record::secondary_key>>
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
			//ram charge to action caller
			_game.emplace(get_self(), [&](auto& game) { 
				game.host = from;
				game.opponent = opponent;
				game.hoststake = quantity;
				game.opponentstake = asset(0.0000,hodl_symbol);
				game.turn = from;
			});
		}
		else if (itrh!=_gameskey.end()) {
			//ram charge to same_payer
			_game.modify(*itrh, same_payer, [&](auto& game) { 
				game.hoststake += quantity;
			});
		}
		else {
			//ram charge to same_payer
			_game.modify(*itrc, same_payer, [&](auto& game) { 
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
		_game.modify(*itr, same_payer, [&]( auto& game ) {
			check(game.is_valid_movement(by, row, col), "invalid move.");
		});
		if (itr->winner!=name()) {
			payback(itr->winner, itr->hoststake+itr->opponentstake, "You got the prize.");
		}
		else if (is_draw(itr->board)) {
			payback(itr->host, itr->hoststake, "Stake refund.");
			payback(itr->opponent, itr->opponentstake, "Stake refund.");
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
	
	void payback(name to, asset quantity, std::string memo) {
		action{
			permission_level{get_self(), "active"_n},
			"eosio.token"_n,
			"transfer"_n,
			std::make_tuple(get_self(), to, quantity, memo)
		}.send();
	}
};
