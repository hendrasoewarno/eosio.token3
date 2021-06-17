actions: [{
  account: "hodl",
  name: "payforcpu",
  authorization: [{
    actor: "payer",
    permission: "active",
  }],
  data: {},
}, {
  account: "eosio.token",
  name: "transfer",
  authorization: [{
    actor: "bob",
    permission: "active",
  }],
  data: {
    from: "tictactoebob",
	to: "hodl",
	quantity: "1.0000 SYS",
	memo: "test"
  }
}]