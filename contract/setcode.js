const target = "JUNGLE";
const fs = require("fs");

const { Api, JsonRpc, Serialize } = require('eosjs');
const { JsSignatureProvider } = require('eosjs/dist/eosjs-jssig');  // development only
const fetch = require('node-fetch');                                // node only
const { TextDecoder, TextEncoder } = require('util');               // node only
//const { TextEncoder, TextDecoder } = require('text-encoding');      // React Native, IE11, and Edge Browsers only

const privateKeys = ['5J5LEuaXy8GDih9UCxMJHdQGY3iyHVjYo2ArP1pQFy74hVKqAgz'];

const signatureProvider = new JsSignatureProvider(privateKeys);

var contractName = 'tictactoe123'
var contractFile = 'tictactoe123'
var rpc = new JsonRpc('http://127.0.0.1:8888', { fetch });

if (target=="JUNGLE")
  rpc = new JsonRpc('http://jungle2.cryptolions.io', { fetch }); //JUNGLE
else
  rpc = new JsonRpc('https://api-kylin.eosasia.one', { fetch }); //KYLIN
  
const api = new Api({ rpc, signatureProvider, textDecoder: new TextDecoder(), textEncoder: new TextEncoder() });

// 1. function to convert wasm to hexstring
function toHex(str) {
    var result = '';
    for (var i=0; i<str.length; i++) {
      hex = str.charCodeAt(i).toString(16);
      result += ("00" + hex).slice(-2);
    }
    return result;
}

// 2. Prepare SETABI
const buffer = new Serialize.SerialBuffer({
  textEncoder: api.textEncoder,
  textDecoder: api.textDecoder,
});
  
(async () => {
  
  try{
    
    const contractDir = '/project/tic-tac-toe/'
    
    //prepare wasm
    const wasm = await fs.readFileSync(contractDir + contractFile + ".wasm","binary")
    const wasmHex = toHex(wasm);
    
    let abi = await JSON.parse(fs.readFileSync(contractDir + contractFile + ".abi", "utf8"))
    
    const abiDefinition = api.abiTypes.get("abi_def")
    
    abi = abiDefinition.fields.reduce(
      (acc, { name: fieldName }) =>
        Object.assign(acc, { [fieldName]: acc[fieldName] || [] }),
      abi
      )
    abiDefinition.serialize(buffer, abi)
    const abiHex = Buffer.from(buffer.asUint8Array()).toString(`hex`);
    
    console.log(wasmHex.substr(0,100));
    console.log(wasmHex.length);
    
    console.log(abiHex.substr(0,100));

    const res1 = await api.transact({
                actions: [
                  {
                    account: 'eosio',
                    name: 'setcode',
                    authorization: [{
                        actor: contractName,
                        permission: 'active',
                    }],
                    data: {
                        account: contractName,
                        vmtype: 0,
                        vmversion: 0,
                        code: wasmHex
                    },
                  }
                ]
            }, {
                blocksBehind: 3,
                expireSeconds: 30,
            });
            
    console.log(res1.processed.action_traces[0]);
    
    const res2 = await api.transact({
                actions: [
                  {
                    account: 'eosio',
                    name: 'setabi',
                    authorization: [{
                        actor: contractName,
                        permission: 'active',
                    }],
                    data: {
                        account: contractName,
                        abi: abiHex
                    },
                  }
                ]
            }, {
                blocksBehind: 3,
                expireSeconds: 30,
            });
    
    console.log(res2.processed.action_traces[0]);    
    
  }catch(e){
    console.log(e);
  }

})();
