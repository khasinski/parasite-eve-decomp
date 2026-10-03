# Draw_AllocColorGradient (main 0x52434, 0x45C bytes): 8 real diffs

Plain C, no pins, barriers, casts or byte-pointer arithmetic. Apply
`headers.diff` to include/pe1/draw_wipe_rect.h, copy the .c to
src/main/main/ and score with offset 52434 size 45C.

What already matches:
- The points loop and the four corner pushes are Draw_EmitWipeBarPoly and
  Draw_AllocColorTriGradient's DRAW_PUSH_WIPE_VERTEX form.
- `&window` stays in s0 across the allocator when SetTexWindow's area goes
  through a block-local `RECT *rect` (DRAW_ADD_TEXTURE_WINDOW).
- Retail loads the next packet cursor above the ordering-table store; that
  needs the cursor/arena as plain scalars (`u32 D_8009D100/D_8009D104`) and
  the ordering-table entry as a record (`DrawPacketAddress *D_8009D11C`,
  `ot->word`), so the in-struct store does not block the scalar load.
- Separate unions per packet (window s1, sprite s0, draw mode s0) and a
  block-local cursor copy for the points path.

Remaining 8 diffs are one block: the sprite fields after the colour `if`.
Retail loads the sprite tag word first (`lw v1,0(s0)`, then X into a0 and Y
into a2, `lui a2,0xff00` after the last field store); stock GCC places the
tag load right before the link (sched1 LAUNCH priority) so Y lands in v1.
Writing y before x fixes the store order (9 -> 8). Tried without effect:
literal masks, tag through `sprite->tag.word`, field stores through a
block-local packet pointer, four other field orders (9 to 12 diffs),
linking before the field stores (30 diffs). The permuter (40k
iterations from this base) only found register-shuffling temporaries.
