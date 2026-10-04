# Render_SetupEntityPrims (main 0x2D850, 0x7DC bytes): parked at lev 4

Typed rewrite of the model-block parser (RenderObjectEntity,
RenderObjectHeader, packet unions; no byte offsets). To build it, apply
`headers.patch` (field names for existing pads in render_object.h:
RenderObjectHeader.vertex_count/matrix_command_bytes,
RenderAnimationLookupEntry.bounds_value0/1, RenderObjectEntity.origin_value;
layout unchanged, but render_object.h is shared by many overlays, so run
the overlay checks before landing it) and copy `render_setup.h` to
include/pe1/. Plain -G0 file, no markers needed.

Score: lev.py main 0x2D850 0x7DC = lev 4 (equal size). Only the
paletteRow/initCount a2/t9 swap is left, see the last section.

What fixed most of it (keep these):
- Read the counts through a COPY of the header parameter (`header = model;`)
  while the cursor union starts from the parameter: retail keeps the header
  in t4 and the section cursor in a1.
- `for (...; i++, desc++)` for the four packet-code loops.
- Command block rounding: `bytes = n; words = (u16)bytes >> 2;
  if ((bytes & 3) > 0) bytes = (words + 1) * 4; else bytes = words * 4;`
  gives retail's srl + andi + blez.
- Lookup entry stores in the order bounds_value0, bounds_value1,
  animation_id (retail base register = entry+6).
- Matrix loop increments the counter first (`for (i = 0; i < n;) { i++; ...`).
- `i = 0;` near the top and `for (; i < packet34_count; ...)`: retail sets
  t2 = 0 before the setup test.

Remaining diffs:
1. FIXED: declare `skipped` as s16. As an int, its zero register was
   reused by cse for the constant in the first loop's `0 < count` test.
2. Params 6/7: retail loads param 7 (initCount) into a2 and param 6 into
   t9; mine swaps them (and the `blez`/`sll t9` pair schedules differently).
3. Texture copy loops: retail copies the cursor into t0 each outer
   iteration (`move t0,a1`) and addresses the record from t0 with raw
   offsets; mine lets loop.c strength-reduce the outer-loop addresses into a
   giv (`addiu t0,a1,6`, offsets -5..7). The giv is created because cse
   folds the `srcquad = cursor.quad` copy back into the biv. Declaring the
   copies first, `cursor.quad = srcquad + 1`, and a for-increment did not
   help; the permuter's best (835) did it with a semantically wrong
   hoisted copy.

Index experiment: `srcquad = &cursor.quad[i]` plus `cursor.quad += i`
after the loop gives retail's raw t0 offsets. But the s16 index
then costs sll/sra/addu each iteration where retail has the `addiu
a1,a1,16` pointer step. Two permuter runs (scratch/a5rsp2, a5rsp3) only
improved the score with a hoisted copy that is never advanced, which
changes the semantics.

## 2026-10-04 (agent 10): lev 33 -> 24, mechanism of the texture loops found

Why mine strength-reduces and retail does not (verified with cc1 -dL and
loop.c): `srcquad = cursor.quad` is a DEST_REG giv of the biv `cursor`,
so every `srcquad->field` load is a DEST_ADDR giv, and combine_givs folds
them into the last one (page_bits, add 6), which is worth reducing. In
retail srcquad is NOT a giv. loop.c only records a DEST_REG giv when the
register is set once in the loop (`n_times_set == 1`). Adding a dead
`srcquad++; srctri++;` after each inner loop (so each copy is set twice)
gives lev 4 and fixes both loops completely, including retail's
`move t0,a1` and the `addiu a1,a1,16` in the delay slot. That dead store
only steers codegen, so it is not committed. A natural source where the
copy is set twice per outer iteration while `cursor` stays a plain
`cursor++` biv is what is still missing.

Forms that do not work:
- `cursor.quad = srcquad + 1` at the end (both loops): cursor stops being
  a biv, no reduction, right loop shape, but global alloc coalesces
  srcquad and cursor into a1, so every register in the function shifts
  (lev 128).
- `srcquad = cursor.quad++;` at the top: right for the triangle loop (kept,
  lev 24; only the a1 += 12 position differs), but the quad loop still
  reduces into `t0 = a1 - 10`.
- Assigning srcquad inside the inner j loop: loop.c hoists it, still a giv.

Params 6/7 (4 of the remaining lev): global alloc takes initCount (pseudo
83) before paletteRow (81) (priority 2 refs over 264 vs 267 insns), but
skips a2 because y prefers it (y goes to a2 as the call's third
argument), so initCount gets t9 and paletteRow gets a2 in the second
pass. Retail is the other way round. Not fixed by `setup && initCount > 0`,
`initCount >= 1`, a block-local copy of paletteRow, or int/s16 for the
callee's palette_row (u16 breaks the sign extension).

## Texture loops solved (lev 24 -> 4)

One source cursor `RenderModelCursor src` serves both texture loops. It
starts as `src = cursor; *textureOut = src.commands;` (the texture section
start), and each loop does `src.quad = cursor.quad;` (or `src.tri`) at the
top and `cursor.quad++` (or `cursor.tri++`) at the end. The copy is now
set before the loop as well, so loop.c finds it is not replaceable
(regno_first_uid is outside the loop) and does not strength-reduce the
reads through it. Both loops are byte-identical: `move t0,a1` at the top
and `addiu a1,a1,16/12` in the back-branch delay slot. No dead stores.

## paletteRow / initCount (the remaining lev 4)

Global alloc priority is 10000 * refs * log2(refs) / live_length,
truncated. Both have 2 refs. paletteRow (pseudo 81) lives 267 insns and
initCount (83) 264, so they score 74 and 75: initCount is allocated first,
skips a2 (y and two other pseudos prefer a2), and takes t9. paletteRow
then gets a2 in the second pass. Retail needs paletteRow first. A tie
would do it, because ties go to the lower pseudo number (81): paletteRow
at 266 or less, or initCount at 267 or more. The gap comes from
paletteRow being defined one insn earlier in the entry block and its last
use (the stack argument) being two insns after the initCount test.
Not fixed by: `initCount >= 1`, `!(initCount <= 0)`, `(s16)paletteRow`,
a block copy of paletteRow before or after the test,
`if (setup && initCount > 0)` outside the block, or int/s16/u16 for the
callee's palette_row.
