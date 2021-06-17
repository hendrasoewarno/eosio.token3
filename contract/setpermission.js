const target = "JUNGLE";
const { Api, JsonRpc } = require('eosjs');
const { JsSignatureProvider } = require('eosjs/dist/eosjs-jssig');  // development only
const fetch = require('node-fetch');                                // node only
const { TextDecoder, TextEncoder } = require('util');               // node only
//const { TextEncoder, TextDecoder } = require('text-encoding');      // React Native, IE11, and Edge Browsers only

//ownerkey
const privateKeys = ['5JeVz3wkQYdcowNAJKYvRcavn6gutuJ42h4ARenAR8YF6d8B8rB'];

const signatureProvider = new JsSignatureProvider(privateKeys);

var contractName = 'tictactoe123'

if (target=="JUNGLE")
  rpc = new JsonRpc('http://jungle2.cryptolions.io', { fetch }); //JUNGLE
else
  rpc = new JsonRpc('https://api-kylin.eosasia.one', { fetch }); //KYLIN

const api = new Api({ rpc, signatureProvider, textDecoder: new TextDecoder(), textEncoder: new TextEncoder() });

//cleos set account permission tictactoe123 active '{"threshold": 1,"keys": [{"key": "EOS86T3RjUMHa9YoJq394jMuaF5hDAZVmeBdpoF4KzwLekbDYgHgz","weight": 1}], "accounts": [{"permission":{"actor":"tictactoe123","permission":"eosio.code"},"weight":1}]}' -p koin2kontrak@owner

(async () => {
  
  try{
    
    auth_obj = {"accounts":[{"permission":{"actor":contractName,"permission":"eosio.code"},"weight":1}],"keys":[{"key":"EOS86T3RjUMHa9YoJq394jMuaF5hDAZVmeBdpoF4KzwLekbDYgHgz","weight":1}],"threshold":1,"waits":[]};
    
    
    const res = await api.transact({
      actions: [{
        account: 'eosio',
        name: 'updateauth',
        authorization: [{
          actor: contractName,
          permission: 'owner',
        }],
        data: {
             account: contractName,
             permission: 'active',
             parent: 'owner',
             auth: auth_obj
          }
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