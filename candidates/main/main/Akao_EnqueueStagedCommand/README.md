# Akao_EnqueueStagedCommand (0x8008CBA8, yaml `[0x7D3A8, asm, main/Akao_EnqueueStagedCommand]`, 0x3C8 bytes)

Executor behind the Akao_Cmd_* wrappers: switches on the staged opcode
(g_AkaoCmdOpcode, D_800BCD80) and fills one or two queue messages via
Akao_AllocMessageSlot. Sample-load opcodes 0x10/0x12/0x19 validate the header
at g_AkaoCmdArg0, skip to the bank id (+4) and reverb parameter (+8), return
the bank id; 0x24 stamps a rolling 0x400..0x5FF sequence (D_8009CDF0); 0xD8/
0xD9/0xDA and 0x98/0x99 emit paired opcodes; the default copies all four args.

Needs `headers.diff`: AkaoStagedMessage (word opcode, union arg0), the linker
alias `Akao_AllocStagedMessage = Akao_AllocMessageSlot` (the queue entry is
declared with a byte opcode for the battle queue, retail stores a word), the
`g_AkaoCmdSampleData` alias for g_AkaoCmdArg0, and D_8009D268/D_8009CDF0.

Status: not matched (stock GCC 2.7.2).

- What already matches: switch tree (unsigned opcode, sltiu), the sample case
  (data pointer walked with `data += 2` reads, `staged = &g_AkaoCmdOpcode` gives
  retail's `la s2`, bank-equal test written as `if (bank_id != result) ... else
  result = 0`), the opcode pairs.
- `scalar_globals.c`: every other case hoists all g_AkaoCmdArg* loads above the
  message stores. Retail serialises each load after the previous message store
  (`lw; nop; sw` chains). In sched.c true_dependence a struct-field store through
  a varying pointer never conflicts with a scalar fixed load, so retail's loads
  must be in-struct.
- `record_globals.c`: reading the args as one-field records reproduces the
  serialised chains and the 0x24 order, but cse now keeps the record address in
  a saved register when the same arg is read on both sides of the second alloc
  call (0xD9/0xDA cases: `la s2`), which retail does not, and the extra live
  register shifts result/staged to s3/s1.

Next idea: find a load form that is MEM_IN_STRUCT but whose address cse does not
promote to a register across the call (e.g. args as one struct read through a
pointer only in the 0x24/default cases), or a message-store form that is not
MEM_IN_STRUCT (stores through a plain `int *` with no PLUS in the tree).
