# Render_SetupColorTable — current candidate (2026-10-05)

**Not matched.** The current candidate has decomp.me-compatible asm-differ
Levenshtein score **140**, 644 bytes, and four differing instructions. It remains
excluded from the build; `main/task3_tail` still uses the original ASM.
The earlier source, rescored with the same configuration, had score 2260,
648 bytes and 87 differing instruction rows. Historical `lev` figures below
are instruction edit distances, not this weighted score.

Only the two constant-load pairs at `800375F4..80037600` are reversed:
retail loads the division reciprocal into t5 before loading the digit-table
address into t7; the candidate loads the address first. The rest of the function,
including registers and branch destinations, matches. There is no byte match yet.

The source uses the existing shared `TextboxEntry` and `TextboxNumber` types.
It includes the adjacent `textbox_open.h` for the rectangle and byte globals.
The public signature remains `(int, int, short *)`, with an explicit byte-sized
style local; no prototype patch or production-header change is required.
The obsolete `headers.diff` has been removed.

Candidate debt: **six register pins and two empty barriers**. Values use t4;
the page index and slot initialization reuse t1; the initial number uses a3;
the first and subsequent quotient copies use a1/a2; the digit counter uses t0.
A memory operand plus an a2 clobber keeps one digit pointer across the inner
loop. A final count operand prevents the count-store expression from overwriting
t0. A scoped `int terminator = -1` allows the sentinel constant to be hoisted.
There are no CPU instruction ASM bodies and no modified toolchain components.

Verified on darwine with stock native GCC 2.7.2:
`-O2 -G8 -funsigned-char -mips1 -mcpu=3000`, stock MASPSX 2.56 mode with
`--dont-force-G0 --use-comm-section`, and GP `0x8009CD70`.
Target: main offset `0x27DE0`, address `0x800375E0`, size `0x284`;
target bytes SHA-256 `d7f8b96cf54e7f4e98ec49c081059b5e47916ce9e25a5f9e97294fd1eb58d58a`.
The exact saved source was recompiled as `review_candidate` and scored again.

Two bounded 24-worker permuter runs on darwine completed 101,921 and 95,340
trials respectively; both have stopped. Manual pin/barrier refinement produced
the score-140 candidate. Separating the first division from the digit store
fixes constant order but moves five arithmetic instructions ahead of the address
calculation (`sched1.c`, score 300). This is the second useful search seed.
Native GCC 2.8.1 and 2.95.2 probes were worse. All scratch results are under
`scratch/Render_SetupColorTable` and the corresponding directory in
`/home/hasik/fx-search-archives` on darwine.

## Historical research (superseded candidate and integration instructions)

### Original draft (main 0x27DE0, yaml `main/task3_tail`, 0x284 bytes)

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

## Why style is not in a1 (agent 18, 2026-10-04): still lev 87

`-dg` on the parked .c (pseudos 72 index, 74 style, 76 values, 77 i, 86/87
the i*56 offset): every global pseudo, style included, conflicts with hard
regs v0..a1, because local-alloc puts block temporaries there and global
records a conflict with each local hard reg live at a pseudo's birth. In the
setup block the temporaries are v0 = the `-1` for `D_8009CEA4`, v1 = flags,
a1 and a0 = the two masks. Retail has hoisted the -1 to t6, so its setup
block needs only v0 (flags), a0 and v1 (masks) and leaves a1 free.

With the -1 hoisted (the permuter's `int minus = -1; D_8009CEA4 = minus;`,
lev 63, still not used) style does get a1 and its copy disappears, so the
-1 hoist is the first thing to solve. The rest of retail's numbering then
follows one more decision: global allocates the quotient (232) before the
offset (87) and values (76). Pass 0 of find_reg gives the quotient a3,
because a3 is already used by local pseudos and a2 is excluded as
"preferred by a conflicting allocno" (values prefers a2 from its entry
copy). The offset then takes a3 in pass 0 too, and values keeps a2. Retail
has the quotient copy in a2, the offset in a2, i in a3 and values copied
to t4, which is what happens when values does not hold the a2 preference
against the slot-loop pseudos. `short *values = numbers;` as a local cursor
was tried (combine folds the two entry copies; no change, lev 87 / 63).
