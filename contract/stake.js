//https://developers.eos.io/manuals/eosjs/v21.0/how-to-guides/how-to-stake
const target = "JUNGLE";
const { Api, JsonRpc } = require('eosjs');
const { JsSignatureProvider } = require('eosjs/dist/eosjs-jssig');  // development only
const fetch = require('node-fetch');                                // node only
const { TextDecoder, TextEncoder } = require('util');               // node only
//const { TextEncoder, TextDecoder } = require('text-encoding');      // React Native, IE11, and Edge Browsers only

//activekey
const privateKeys = ['5JL1gjsq8B2hWjADdxtdAxP4qTMDdc7UYSGp4ZjGq6EJtH979vD'];

const signatureProvider = new JsSignatureProvider(privateKeys);

var contractName = 'tictactoebob'

if (target=="JUNGLE")
  rpc = new JsonRpc('http://jungle2.cryptolions.io', { fetch }); //JUNGLE
else
  rpc = new JsonRpc('https://api-kylin.eosasia.one', { fetch }); //KYLIN

const api = new Api({ rpc, signatureProvider, textDecoder: new TextDecoder(), textEncoder: new TextEncoder() });

(async () => {
  
  try{
    
    const res = await api.transact({
      actions: [{
        account: 'eosio',
        name: 'delegatebw',
        authorization: [{
          actor: contractName,
          permission: 'active',
        }],
        data: {
            from: contractName,
            receiver: contractName,
            stake_net_quantity: '1.0000 EOS',
            stake_cpu_quantity: '1.0000 EOS',
            transfer: false,
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