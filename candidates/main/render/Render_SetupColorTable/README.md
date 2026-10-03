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
