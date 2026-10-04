# Menu_ItemListInputHandler (0x800494AC, 0xC1C bytes, main.yaml 0x39CAC)

Plain C, stock GCC 2.7.2, no pins, barriers, volatile or casts to integers.
Same size and frame as retail (3100 bytes, 0x40 frame); 21 words differ,
all register choices. Control flow, scheduling and every tail match.

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

Remaining differences:

1. Global allocation swaps `child` (retail s1) and `data` (retail s2).
   Priorities from cc1 -dl: data 8 refs / 115 insns beats child 6 / 82, and
   the CF1C branch's `usable` (3 refs / 30 insns) loses s1 to data. Giving
   `usable` one more ref (`usable++` instead of `usable = 1`) makes it take
   s1 first and the whole swap disappears (5 diffs, but `addiu s1,s1,1`
   instead of `li s1,1`). The permuter only found a duplicated dead arm.
   The source form that gives retail its extra ref is still unknown.
2. `slots` (0x1000) and `equip` (0x40) test v0 instead of a0: sched1 hoists
   the `a0 = node` argument copy above the stores, so the pointer ties to
   v0. Retail keeps the copy below the `li v0,-1`, so the pointer lives in a0.
