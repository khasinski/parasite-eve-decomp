# Scene_LoadRoom (0x8006B4F8, 0x870 bytes, main.yaml 0x5BCF8)

Typed plain-C draft, stock GCC 2.7.2 + maspsx, `-G1` for both cc1 and maspsx
(the one-byte room letter `D_8009CDC8` is gp-relative, every 4-byte global is
absolute). No pins, barriers, volatile or integer casts. Three `goto` CD retry
restarts (recorded debt when it lands). Score: **lev 83** (retail 540 words,
mine 537 words).

Types live in `include/pe1/scene_room.h` (room directory, 12/8-byte records,
stream records, sector ranges); `g_GameState` gained `room_type` (0x08) and
`texture_load_scratch` (0x168), and its bank slot tables are pointer typed.

What already matches:

- Control flow of the three read/poll phases: retry targets, the TIM upload
  inside the poll loop, `&tim[i]` index form (loop hoisting of 0x3FFFFF is
  avoided), `sltu` against `loaded` for the TIM loop entry (cse makes the
  counter equivalent to `loaded` only because `loaded` is referenced again at
  the end: it is the `1` passed to the first CD_ReadSectors).
- Map number spilled to the stack like retail (one more callee-saved pseudo).
- `SCENE_ROOM_PAYLOAD` macro: the destination slot address is computed before
  the payload pointer; an inline call on the right-hand side reorders it.
- Script table: `index = slot - 0x55` keeps retail's unfolded subtract.
- Stream records: the store reads the bank back as a byte (`bank.low`).
- Tail: `(flags & 1) && bank != 0` else `!(flags & 0x10)` shapes.

Remaining differences (lev 83):

1. Global allocation swaps `ready` (retail s1) and the loop counter `i`
   (retail s2): about 40 words. cc1 -dl: i 78 refs / 241 insns (1.94) beats
   ready 21 refs / 63 insns (1.33); a separate TIM counter is even denser
   (22 / 32). Retail needs `ready` allocated first.
2. Frame is 0x88 instead of 0x80 (all save/restore offsets, ~22 words): GCC
   leaves one 8-byte stack slot per folded loop-entry `sltu` pseudo (7 here,
   retail has 6), so one loop entry test is shaped differently in retail.
3. Script table: retail copies `slot` into a0 before the range test and does
   the first `sll` after it (about 5 words).
4. Samples table: retail `lhu v0; move v1,v0` before `andi` (1 word); sample
   bank compare loads the record before the state (2 words).
