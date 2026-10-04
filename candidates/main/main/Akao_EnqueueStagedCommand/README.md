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

## Struct staging view (agent 12, 2026-10-04): lev 16

`struct_staging.c` (retail 242 words, mine 242) uses only types and plain C,
with no aliases. Its declarations at the top are a scratch view, and it
compiles standalone: score with offset 7D3A8 size 3C8. What decided it:
- The staging area is ONE struct `{ int opcode; StagedArg args[4]; }` at
  0x800BCD80, with `StagedArg` a union of int and `unsigned short *`. That
  makes every arg read in-struct, so it stays below the previous message
  store, and args at a non-zero offset are read with lui/lw rather than a
  `la` kept across the alloc call. With args as a separate array, args[0]
  sits at the bare symbol and cse keeps it in s0 (lev 83). Scalars give
  lev 95.
- Sample case: `staged = &g_AkaoCmd.opcode;` BEFORE `data = args[0].data;`.
  In the other order, cse derives &opcode from the args[0] address
  (`la s1,+4`, `-4(s1)`) (lev 40 -> 20).
- 0xD9/0xDA: store the arg BEFORE the opcode (`arg1 = ...; opcode = 0xD1;`)
  (lev 69 -> 43 with the other fixes).
- 0x24: the sequence counter D_8009CDF0 as a one-field record, order
  arg0, arg1, arg2, opcode, `result = seq`, seq store, sequence, arg3
  (best of all 6! orders, lev 16; the scalar counter is at best lev 20).

Remaining (lev 16, all in case 0x24): the record makes cse keep
`&D_8009CDF0` in a0 for the load and the store (retail uses lui/lw and
lui at/sw). Retail also loads the counter into v0 and copies it
(`move s1,v0`), and loads arg3 before the counter store.

Blockers to landing it, which are header work rather than codegen:
1. g_AkaoCmdOpcode..g_AkaoCmdArg3 are five scalars used about 130 times in
   Akao_CommandStaging.c, Akao_Cmd_99_9B_9D.c, Akao_CommandEncoders.c and
   Akao_CommandModifiers.c. Turning them into the struct means rewriting
   those matched units, then rechecking them (the in-struct alias rule can
   change their scheduling).
2. The message is filled with a word opcode, but AkaoQueueEntry (typedef of
   the battle SquareMessageEntry) has a byte opcode. It needs an AKAO queue
   entry type of its own (word opcode at 0, args at 4..0x14). That means
   moving the AkaoQueueEntry typedef out of battle_cmd.h, which is battle
   code, so it needs the owner's approval.
