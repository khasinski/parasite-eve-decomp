# Render_SetupEntityPrims (main 0x2D850, 0x7DC bytes): parked at 31 diff lines

Typed rewrite of the model-block parser (RenderObjectEntity,
RenderObjectHeader, packet unions; no byte offsets). To build it, apply
`headers.patch` (field names for existing pads in render_object.h:
RenderObjectHeader.vertex_count/matrix_command_bytes,
RenderAnimationLookupEntry.bounds_value0/1, RenderObjectEntity.origin_value;
layout unchanged, but render_object.h is shared by many overlays, so run
the overlay checks before landing it) and copy `render_setup.h` to
include/pe1/. Plain -G0 file, no markers needed.

Score: `ds.py ... 2D850 7DC` = 31 diff lines (started at 317).

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
1. First loop entry test: mine `slt v0,s2,v0` (cse substitutes the
   `skipped = 0` register for i's constant 0), retail `beqz` on the count.
   Moving `skipped = 0` elsewhere changes the s-register allocation.
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
