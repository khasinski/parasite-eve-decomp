# Render_SetupColorTable (main 0x27DE0, yaml `main/task3_tail`, 0x284 bytes)

Plain C candidate, 648 bytes against retail's 644 (one extra instruction).
Apply `headers.diff` to include/pe1/textbox.h (TextboxEntry gains
`instant` at 0x09 and `numbers[5]` {digits[5], count} at 0x1A; the text
rectangle D_8009CE98 becomes a four-short record; the prototype becomes
`(short index, unsigned char style, short *values)`).

The function opens the first free textbox (4 entries, stride 0x38), copies
the text rectangle into styled boxes, and writes up to five -1 terminated
values as least-significant-first decimal digits.

Settled so far:
- `unsigned char style` parameter gives retail's `andi a1,0xff` test and the
  later `lbu` reload for `style == 3`.
- Two statements `flags &= ~0x100000; flags &= ~0x200000;` give the two
  separate `and`s with one store (a single expression folds the masks).
- The rectangle must be one record: retail keeps each `lhu` below the
  previous field store.
- `short value; unsigned short quotient;` gives the `andi 0xffff` quotient
  multiply and the `sll 16` zero tests.

Remaining:
- Retail hoists `-1` (t6, shared by `D_8009CEA4 = -1` and the terminator
  compare) and `&g_TextboxEntries[0].numbers` (t7) to the function entry;
  here the -1 stays in the slot loop (QImode store constant vs SImode
  compare do not share a pseudo).
- Retail forms the digit base as `(slot*6 + i*56) + base` per slot with
  `i*56` computed after the style block; here `i*56 + base` is hoisted.
  Direct `numbers[slot].digits[k]` stores are worse (656 bytes).

## Rebased on main (agent 4, 2026-10-04): still 648 vs 644

main's textbox.h now has `background` (0x09), `control.flags`, `width` and
`height`; the candidate uses those names. `headers.diff` now only retypes
the prototype to `(short index, unsigned char style, short *values)`, and the
rectangle plus the three byte globals moved to a narrow header
`textbox_open.h` (goes to include/pe1/), because Menu_SetTextCursorRect.c and
Render_SetupFogLayer.c still declare those symbols themselves.

Why the -1 is not hoisted (cc1 -dL): the slot loop has 74 real insns, so its
compare constant (insn 355, life 1, savings 1) is `not desirable` at
threshold 58. In the outer loop, loop.c merges it with the QImode -1 of the
`D_8009CEA4 = -1` store (combine_movables lets the wider SImode movable absorb
the narrower one, life 2 savings 2), but it is visited after the two
movables re-hoisted from the slot loop (the magic 0x66666667 and the digit
base), and each of those doubles insn_count ("halved since already moved"),
so 49 * 2 * 2 = 196 < 484. Retail hoists the -1 to the entry, so in retail
either the slot loop is at most 58 insns at loop time (the -1 is hoisted at
the inner level first, and then sits ahead of the other preheader insns) or
the -1 movable is visited before the re-hoisted ones. Retail also computes
i*56 once per box after the style block (t3) and adds `slot*6` per slot,
then t7 (&numbers base); that shape is a hint for the smaller slot loop.

## Rescored with lev.py (agent 12, 2026-10-04): lev 87

The earlier draft scores lev 92 (retail 161 words, mine 162). Most of the
distance is register numbering (retail: i in a3, values copied to t4, index
in t1, style left in a1), so the size comparison above understated it.
Score from the worktree root after applying headers.diff and copying
textbox_open.h to include/pe1/, with offset 27DE0 size 284.

New in the parked .c (lev 87, 162 words):
- Digit stores written as direct lvalues,
  `g_TextboxEntries[i].numbers[slot].digits[count] = value - (quotient = value / 10) * 10;`
  for the first digit (count is 0) and the same form in the loop. This is
  retail's digit base: get_inner_reference builds the offset as
  `slot*6 + i*56` and adds the base symbol last, so the base is
  `(slot*6 + t3) + t7`, computed before the division, and the inner loop
  adds count to it. A `digits` pointer variable goes through the c-typeck
  `&x[y] -> x + y` chain, which gives `(i*56 + base) + slot*6`. The
  `i*56 + base` part is then hoisted out of the slot loop.
- Remaining differences: the base pseudo is copied (`move t3,a3`) for the
  inner loop where retail keeps one register (t2). The quotient copy
  `move a1,a0` is missing. The -1 is still not hoisted, and the registers
  are numbered differently all through.

The -1 hoist, found by the permuter (not used, because it is a steering temp):
`int minus = -1; D_8009CEA4 = minus;` makes the store's constant an SImode
movable that comes first in the outer loop. It then absorbs the compare's
-1 before the two re-hoisted movables double insn_count (move_movables
keeps doubling insn_count for every moved_once movable it visits). That
gives retail's `li t6,-1` at entry and lev 63. The magic constant and the
base then come out in the wrong order (base first). A plain-C form whose
store constant is SImode, or whose -1 compare is visited before the
slot-loop preheader movables, should get the same effect.
The slot loop is 74 to 85 insns at loop time against a threshold of 58,
so the -1 cannot be hoisted at the slot level without big changes.
Value/quotient type sweeps (short/int/u16 combinations) were all worse.

## Retry (agent 16, 2026-10-04): still lev 87

Structural toggles scored with lev.py, none below 87: value/quotient/count
and slot declared at function scope, `if (state == 0) { ... return; }`
instead of `continue`, the slot loop reading `values[slot]` or stepping
`values++` in the for header (161 words, but lev 89), `break` instead of
`return` on the terminator, and the `D_8009CEA4 = -1` store moved. The
register differences come from global allocation order: retail leaves
`style` in a1 and copies `values` to t4 and `index` to t1, while this
draft keeps `values` in a2 (its copy preference wins because i*56 lands in
a3 instead of a2).
