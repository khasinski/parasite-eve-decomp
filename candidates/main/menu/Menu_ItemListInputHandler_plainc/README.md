# Menu_ItemListInputHandler (0x800494AC, 0xC1C bytes, main.yaml 0x39CAC)

Plain C, stock GCC 2.7.2, no pins, barriers, volatile or casts to integers.
Same size and frame as retail (3100 bytes, 0x40 frame). Score: **lev 5**
(retail 775 words, mine 775 words). Control flow, scheduling and every tail
match.

What made the rest match:

- The repeated "close item list" sequence (five NavScrollTo calls, focus the
  equipment slots or clear the tab cursors, clear D_8009CF34) and the three
  SetCursorY calls are static inline helpers.
- The usable action panel and the equip selection panel (the same code as
  Menu_CreateEquipItemSelectionView) are inline helpers too; with their own
  block-scoped root/node pointers the registers land like retail and the
  0x40 frame comes out right.
- Stat clamp: `x = x + d >= 1000 ? 999 : x + d` (the else arm is reached by
  a jump, so cse keeps the second addition that dbr puts in the delay slot).
- Each confirm branch ends with its own `Menu_PlayConfirmSound(); return 1;`
  and only the D_8009CF38 branch falls into the shared error tail; jump2
  then keeps the copy after the D_8009CF30 branch like retail.
- `action = Menu_ItemUseAction;` before the width test reproduces retail's
  early `lui s3` (the callback pointer lives across the second measure call).

Remaining differences (lev 5):

0. (2026-10-04, lev 21 -> 5) `usable |= 1;` instead of `usable = 1;` in
   Menu_CanEquipSelection. flow counts the extra use of `usable`, which lifts
   it above `data` in global allocation, so `usable` takes s1, `data` s2 and
   `child` s1 exactly as in retail. The cost is one word: `ori s1,s1,0x1`
   where retail has `li s1,1`. Variants: `usable = !usable` (same lev 5,
   `sltiu`), `if ((usable = Inv_SetupSlotDisplay(...)) != 0) usable = 1;`
   (lev 19: jump threads the zero path and swaps usable/selection), `u8`
   usable, `else usable = 0`, `usable = call() != 0` (lev 29-32). Still
   wanted: a source form with the extra reference that keeps `li`.
1. (fixed by 0 except the `ori`) Global allocation swaps `child` (retail s1) and `data` (retail s2).
   Priorities from cc1 -dl: data 8 refs / 115 insns beats child 6 / 82, and
   the CF1C branch's `usable` (3 refs / 30 insns) loses s1 to data. Giving
   `usable` one more ref (`usable++` instead of `usable = 1`) makes it take
   s1 first and the whole swap disappears (5 diffs, but `addiu s1,s1,1`
   instead of `li s1,1`). The permuter only found a duplicated dead arm.
   The source form that gives retail its extra ref is still unknown.
2. `slots` (0x1000) and `equip` (0x40) test v0 instead of a0: sched1 hoists
   the `a0 = node` argument copy above the stores, so the pointer ties to
   v0. Retail keeps the copy below the `li v0,-1`, so the pointer lives in a0.

Note (2026-10-04, agent 4): why sched1 hoists the `a0 = slots` copy. In the
-dS trace the copy (insn 54) and the two slot stores (46, 51) are all ready
at the same cycle with priority 1; rank_for_schedule then picks "the first
one with the largest potential hazard", and the stores (memory unit) always
beat the unit-less copy, so the copy is placed above them. Only a block
boundary or a different priority can keep it below. Tried without effect
(still 21): every order of the three stores, the store folded into the call
argument (`(slots->cursor_y = 1, slots)`), copying slots into `node` before
or after the stores, two consecutive `if (slots != 0)` blocks, and inline
helpers (stores + call with a row parameter; stores behind an early
`if (slots == 0) return;`; a shared "focus" helper for the slots and equip
paths). The early-return helper's label is deleted by jump before sched1.
