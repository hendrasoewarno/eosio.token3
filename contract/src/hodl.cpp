#include <eosio/eosio.hpp>
#include <eosio/print.hpp>
#include <eosio/asset.hpp>
#include <eosio/system.hpp>
#include <eosio/singleton.hpp>
#include <eosio.token/eosio.token.hpp> // use action wrapper

using namespace eosio;

class [[eosio::contract("hodl")]] hodl : public eosio::contract {
  public:
    using contract::contract;

    hodl(name receiver, name code, datastream<const char *> ds) : contract(receiver, code, ds),
      hodl_symbol("SYS", 4),
      singleton_instance(receiver, receiver.value) {} //singleton initialize

    [[eosio::on_notify("eosio.token::transfer")]]
    void deposit(name hodler, name to, eosio::asset quantity, std::string memo) {
      if (hodler == get_self() || to != get_self())
      {
        return;
      }

      check(now() < get_next_party(), "You're way late");
      check(quantity.amount > 0, "When pigs fly");
      check(quantity.symbol == hodl_symbol, "These are not the droids you are looking for.");

      balance_table balance(get_self(), hodler.value);
      auto hodl_it = balance.find(hodl_symbol.raw());

      if (hodl_it != balance.end())
		//contract pay for the RAM
        balance.modify(hodl_it, get_self(), [&](auto &row) {
          row.funds += quantity;
        });
      else
		//contract pay for the RAM
        balance.emplace(get_self(), [&](auto &row) {
          row.funds = quantity;
        });
    }

    [[eosio::action]]
    void setup(uint32_t nearest_minutes) {
      require_auth(get_self());
      auto stored_setting = singleton_instance.get_or_create(get_self(), default_setting);
      stored_setting.next_party(now(), nearest_minutes);
      singleton_instance.set(stored_setting, get_self());
    }

    [[eosio::action]]
    void party(name hodler)
    {
      //Check the authority of hodlder
      require_auth(hodler);

      // //Check the current time has pass the the party time
      check(now() > get_next_party(), "Hold your horses");

      balance_table balance(get_self(), hodler.value);
      auto hodl_it = balance.find(hodl_symbol.raw());

      // //Make sure the holder is in the table
      check(hodl_it != balance.end(), "You're not allowed to party");

	  //using inline method
      /*
	  //take 1% from withdrawals to help cover the cost
      action{
        permission_level{get_self(), "active"_n},
        "eosio.token"_n,
        "transfer"_n,		
        std::make_tuple(get_self(), hodler, asset(hodl_it->funds.amount*0.99, hodl_symbol), std::string("Party! Your hodl is free."))
      }.send();
      */
      token::transfer_action payback{"eosio.token"_n, {get_self(), "active"_n}};
	  //take 1% from withdrawals to help cover the cost
      payback.send(get_self(), hodler, asset(hodl_it->funds.amount*0.99, hodl_symbol), std::string("Party! Your hodl is free."));

      balance.erase(hodl_it);
    }
	
  private:
    const symbol hodl_symbol;

    //prepare singleton
    struct [[eosio::table]] setting {
      uint32_t the_party;
      void next_party(uint32_t current_time, uint32_t minutes) {
        the_party=((uint32_t)(current_time/(minutes*60))*(minutes*60))+(minutes*60);
      }
    } default_setting;
	
    using singleton_type = eosio::singleton<"setting"_n, setting>;
    singleton_type singleton_instance;

    struct [[eosio::table]] balance
    {
      eosio::asset funds;
      uint64_t primary_key() const { return funds.symbol.raw(); }
    };

    using balance_table = eosio::multi_index<"balance"_n, balance>;

    uint32_t now() {
      return current_time_point().sec_since_epoch();
    }

    uint32_t get_next_party() {
      if (singleton_instance.exists())
        return singleton_instance.get().the_party;
      else
        return 0;
    }	
};
