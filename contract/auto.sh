#!/bin/bash

echo "Start"
eosio-cpp -o eosio.token.wasm src/eosio.token.cpp -I include --abigen
cleos create account eosio eosio.token EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
cleos set account permission eosio.token active --add-code
cleos set code eosio.token eosio.token.wasm
cleos set abi eosio.token eosio.token.abi

cleos create account eosio bob EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
cleos create account eosio jane EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
cleos push action eosio.token create '[ "eosio.token", "1000000000.0000 SYS"]' -p eosio.token@active
cleos push action eosio.token issue '[ "eosio.token", "100.0000 SYS", "memo" ]' -p eosio.token@active
cleos push action eosio.token transfer '[ "eosio.token", "bob", "25.0000 SYS", "m" ]' -p eosio.token@active
cleos push action eosio.token transfer '[ "eosio.token", "jane", "25.0000 SYS", "m" ]' -p eosio.token@active

#eosio-cpp -o hodl.wasm src/hodl.cpp -I include --abigen
#cleos create account eosio hodl EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
#cleos set account permission hodl active --add-code
#cleos set code hodl hodl.wasm
#cleos set abi hodl hodl.abi
#prove singleton
#cleos get table hodl hodl setting
#cleos push action hodl setup '[ 15 ]' -p hodl@active
#cleos get table hodl hodl setting
#prove on_notify
#cleos push action eosio.token transfer '[ "bob", "hodl", "1.0000 SYS", "m" ]' -p bob@active
#cleos push action hodl party '[ "bob" ]' -p bob@active
#cleos get table hodl bob balance

eosio-cpp -o tictactoe.wasm src/tictactoe.cpp -I include --abigen
cleos create account eosio tictactoe EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
cleos set account permission tictactoe active --add-code
cleos set code tictactoe tictactoe.wasm
cleos set abi tictactoe tictactoe.abi

#cleos get table eosio.token tictactoe accounts
#cleos push action eosio.token transfer '[ "bob", "tictactoe", "1.0000 SYS", "jane" ]' -p bob@active
#cleos get table tictactoe tictactoe games
#cleos get table eosio.token tictactoe accounts
#cleos push action tictactoe close '{"payload": {"opponent":"jane", "host":"bob"}}' -p bob@active
#cleos push action eosio.token transfer '[ "jane", "tictactoe", "1.0000 SYS", "bob" ]' -p jane@active
#cleos push action tictactoe move '{"payload": {"opponent":"jane", "host":"bob", "by":"bob", "row":1, "col":0}}' -p bob@active
#cleos push action tictactoe move '{"payload": {"opponent":"jane", "host":"bob", "by":"jane", "row":0, "col":0}}' -p jane@active
#cleos push action tictactoe move '{"payload": {"opponent":"jane", "host":"bob", "by":"bob", "row":1, "col":1}}' -p bob@active
#cleos push action tictactoe move '{"payload": {"opponent":"jane", "host":"bob", "by":"jane", "row":0, "col":1}}' -p jane@active
#cleos push action tictactoe move '{"payload": {"opponent":"jane", "host":"bob", "by":"bob", "row":1, "col":2}}' -p bob@active
#cleos push action tictactoe move '{"payload": {"opponent":"jane", "host":"bob", "by":"jane", "row":0, "col":2}}' -p jane@active

#cleos push action eosio.token transfer '[ "bob", "tictactoe", "1.0000 SYS", "jane" ]' -p bob@active
#cleos get table tictactoe tictactoe games
#cleos get table eosio.token tictactoe accounts
#cleos push action tictactoe close '{"payload": {"opponent":"jane", "host":"bob"}}' -p bob@active
#cleos push action eosio.token transfer '[ "jane", "tictactoe", "1.0000 SYS", "bob" ]' -p jane@active
#cleos push action tictactoe move '{"payload": {"opponent":"jane", "host":"bob", "by":"bob", "row":0, "col":1}}' -p bob@active
#cleos push action tictactoe move '{"payload": {"opponent":"jane", "host":"bob", "by":"jane", "row":0, "col":0}}' -p jane@active
#cleos push action tictactoe move '{"payload": {"opponent":"jane", "host":"bob", "by":"bob", "row":1, "col":0}}' -p bob@active
#cleos push action tictactoe move '{"payload": {"opponent":"jane", "host":"bob", "by":"jane", "row":1, "col":1}}' -p jane@active
#cleos push action tictactoe move '{"payload": {"opponent":"jane", "host":"bob", "by":"bob", "row":1, "col":2}}' -p bob@active
#cleos push action tictactoe move '{"payload": {"opponent":"jane", "host":"bob", "by":"jane", "row":0, "col":2}}' -p jane@active
#cleos push action tictactoe move '{"payload": {"opponent":"jane", "host":"bob", "by":"bob", "row":2, "col":2}}' -p bob@active
#cleos push action tictactoe move '{"payload": {"opponent":"jane", "host":"bob", "by":"jane", "row":2, "col":1}}' -p jane@active
#cleos push action tictactoe move '{"payload": {"opponent":"jane", "host":"bob", "by":"bob", "row":2, "col":0}}' -p bob@active
