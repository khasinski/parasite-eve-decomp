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
