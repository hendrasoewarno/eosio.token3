const target = "JUNGLE";

const { Api, JsonRpc } = require('eosjs');
const { JsSignatureProvider } = require('eosjs/dist/eosjs-jssig');  // development only
const fetch = require('node-fetch');                                // node only
const { TextDecoder, TextEncoder } = require('util');               // node only
//const { TextEncoder, TextDecoder } = require('text-encoding');      // React Native, IE11, and Edge Browsers only

var privateKeys = [];
privateKeys['tictactoebob'] = ['5JL1gjsq8B2hWjADdxtdAxP4qTMDdc7UYSGp4ZjGq6EJtH979vD'];
privateKeys['tictactoejan'] = ['5JVpu92dSWw46hVJib8QybLk8ixz9yjmTt8ogcWvYdJjjZUwCUJ'];

if (target=="JUNGLE")
  rpc = new JsonRpc('http://jungle2.cryptolions.io', { fetch }); //JUNGLE
else
  rpc = new JsonRpc('https://api-kylin.eosasia.one', { fetch }); //KYLIN

function pushAction(rpc, contract, action, actor, data) {
  const signatureProvider = new JsSignatureProvider(privateKeys[actor]);
  const api = new Api({ rpc, signatureProvider, textDecoder: new TextDecoder(), textEncoder: new TextEncoder() });

  (async () => {
    try{    
      const res = await api.transact({
        actions: [{
          account: contract,
          name: action,
          authorization: [{
            actor: actor,
            permission: 'active',
          }],
          data: data
        }]
      }, {
        blocksBehind: 3,
        expireSeconds: 30,
      });
    
      console.log(res.processed.action_traces[0]);

    }catch(e){
      console.log(e);
    }

  })();
}

pushAction(rpc, 'eosio.token', 'transfer', 'tictactoebob', {from:'tictactoebob', to:'tictactoe123', quantity:'1.0000 EOS', memo:'tictactoejan'});
pushAction(rpc, 'eosio.token', 'transfer', 'tictactoejan', {from:'tictactoejan', to:'tictactoe123', quantity:'1.0000 EOS', memo:'tictactoebob'});
pushAction(rpc, 'tictactoe123', 'move', 'tictactoejan', {opponent:'tictactoebob', host:'tictactoejan', by:'tictactoejan', row: 1, col:0});
pushAction(rpc, 'tictactoe123', 'move', 'tictactoebob', {opponent:'tictactoebob', host:'tictactoejan', by:'tictactoebob', row: 0, col:0});
pushAction(rpc, 'tictactoe123', 'move', 'tictactoejan', {opponent:'tictactoebob', host:'tictactoejan', by:'tictactoejan', row: 1, col:1});
pushAction(rpc, 'tictactoe123', 'move', 'tictactoebob', {opponent:'tictactoebob', host:'tictactoejan', by:'tictactoebob', row: 0, col:1});
pushAction(rpc, 'tictactoe123', 'move', 'tictactoejan', {opponent:'tictactoebob', host:'tictactoejan', by:'tictactoejan', row: 1, col:2});
pushAction(rpc, 'tictactoe123', 'move', 'tictactoebob', {opponent:'tictactoebob', host:'tictactoejan', by:'tictactoebob', row: 0, col:2});

/*
cleos push action tictactoe move '[jane, bob, bob, 1,0]' -p bob
cleos push action tictactoe move '[jane, bob, jane, 0,0]' -p jane
cleos push action tictactoe move '[jane, bob, bob, 1,1]' -p bob
cleos push action tictactoe move '[jane, bob, jane, 0,1]' -p jane

# Bob Won the game
cleos push action tictactoe move '[jane, bob, bob, 1,2]' -p bob

# Error Game Over
cleos push action tictactoe move '[jane, bob, jane, 0,2]' -p jane
*/
