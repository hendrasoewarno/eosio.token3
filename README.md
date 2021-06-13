## Exercise EOSIO for Ethereum Developers 101

Create a Tic Tac Toe 
- Welcome action can only be taken by contract account
- create( const name &challenger, name &host );
- close( const name &challenger, const name &host );
- restart( const name &challenger, const name &host, const name &by );
- move( const name &challenger, const name &host, const name &by, const uint16_t &row, const uint16_t &column ); 

Make the create action payable;  name host  must send in tokens to start a game, naming an opponent in the memo.  The opponent must send in tokens with host in memo in order to accept a game. If a non-matching amount or non-existent host is submitted, the action is rejected. Close cannot be called once a game is accepted, repay the host when the close action is called. Build a move action that can be called by the player with the current active turn. When a player wins, the pot is sent to that player. In the case of the tie, both players are refunded.

## Setup

1. Fork this repo to your personal GitHub account so that you can save your work into your personal Github account.

2. Open https://gitpod.io#your_github_repo

## Installing the contract

Run this in a terminal:

```
cleos create account eosio eosio.token EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
cleos get account eosio.token

## this contract needs an eosio.code permission
cleos set account permission eosio.token active --add-code
cleos get account eosio.token

cleos set code eosio.token eosio.token.wasm
cleos set abi eosio.token eosio.token.abi
```

## Creating users and using the contract

Run this in a terminal:
```
cleos create account eosio bob EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
cleos create account eosio jane EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV

## Create SYS Token, and eosio.token as issuer
cleos push action eosio.token create '[ "eosio.token", "1000000000.0000 SYS"]' -p eosio.token@active

cleos get table eosio.token SYS stat

## Issue 100 SYS Token to eosio.token account
cleos push action eosio.token issue '[ "eosio.token", "100.0000 SYS", "memo" ]' -p eosio.token@active

cleos get table eosio.token SYS stat

## Transfer 25 SYS Token from eosio.token to bob
cleos push action eosio.token transfer '[ "eosio.token", "bob", "25.0000 SYS", "m" ]' -p eosio.token@active

## Transfer 15 SYS Token from bob to jane
cleos push action eosio.token transfer '[ "bob", "jane", "15.0000 SYS", "m" ]' -p bob@active

cleos get table eosio.token eosio.token accounts
cleos get table eosio.token bob accounts
cleos get table eosio.token jane accounts
```

## Tic-Tac-Toe
```
eosio-cpp -o tictactoe.wasm src/tictactoe.cpp -I include --abigen
cleos create account eosio tictactoe EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
cleos set account permission tictactoe active --add-code
cleos set code tictactoe tictactoe.wasm
cleos set abi tictactoe tictactoe.abi

cleos get table eosio.token tictactoe accounts

# Bob as Host
cleos push action eosio.token transfer '[ "bob", "tictactoe", "1.0000 SYS", "jane" ]' -p bob@active
cleos get table tictactoe tictactoe games
get table eosio.token tictactoe accounts

# Jane as Opponent
cleos push action eosio.token transfer '[ "jane", "tictactoe", "1.0000 SYS", "bob" ]' -p jane@active
cleos get table tictactoe tictactoe games
cleos push action tictactoe close '[ "jane", "bob" ]' -p bob@active
cleos get table eosio.token tictactoe accounts
cleos push action tictactoe move '[jane, bob, bob, 1,0]' -p bob
cleos push action tictactoe move '[jane, bob, jane, 0,0]' -p jane
cleos push action tictactoe move '[jane, bob, bob, 1,1]' -p bob
cleos push action tictactoe move '[jane, bob, jane, 0,1]' -p jane

# Bob Won the game
cleos push action tictactoe move '[jane, bob, bob, 1,2]' -p bob

# Error Game Over
cleos push action tictactoe move '[jane, bob, jane, 0,2]' -p jane
```

## Resetting the chain

To remove the existing chain and create another:

* Switch to the terminal running `nodeos`
* Press `ctrl+c` to stop it
* Run the following

```
rm -rf ~/eosio/chain
nodeos --config-dir ~/eosio/chain/config --data-dir ~/eosio/chain/data -e -p eosio --plugin eosio::chain_api_plugin --contracts-console
