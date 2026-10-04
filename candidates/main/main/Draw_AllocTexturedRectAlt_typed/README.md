# Draw_AllocTexturedRectAlt (main 0x50E2C, 0xA18 bytes): parked at lev 275

Typed first pass at the stat value + level bar renderer (-G8/-G8, same
markers as Draw_AllocSprite and the other draw units). Copy
`draw_level_bar.h` to include/pe1/ to build it. There is no flags-word
problem: every global is either gp-relative or a cursor-stack bound
(address only).

Structure, as decoded from the asm:
- Glyph 0x48 descriptor; push the cursor; then move to (x, y + 2) and push
  again. Retail stores the second push's data before the stack pointer, the
  first push in the other order. That needs two different helpers
  (PushCursor, PushCursorAt), as in Draw_TexturedNumberPrint.
- Print the value at (x + 0x27, y - 2) with colours 0x808080 / 0x404040,
  pop, sprite 0x49.
- Six bar slices (textured quads, length 9, code 0x2C), each conditional on
  width: < 0x2F, < 0x2E, < 0x30, > 0, >= 3, >= 2. Left edges are X+1, X+2,
  X+0x30-w, X+0x31-w, X+0x32-w, X+0x31. Right edges are computed from the
  stored x0: +1, +0x2E-w (retail: (x0+0x2E)-w), +1, +1, (x0-1)+w, +1. The
  u offsets are 0..5. Height comes from the descriptor (y and v). Stores
  follow PSY-Q style chained assignments (`x1 = x3 = x0 + ..`, `y0 = y1 =`).
- Pop, then a draw-mode packet with tpage ((mode & 3) << 7) | 7, linked
  into the OT even when the allocation failed.

Main remaining gap: retail copies each slice pointer into a3 (`move a3,s0`
in both arms after the colour setup). It allocates with s0 (live across the
assert call) and fills the fields through a3. Every C form tried (inline
allocator returning the pointer, explicit copy) lets cse merge the copy,
so all field stores use s0 and the registers shift. The matched sibling
Draw_EmitDigitSprite uses `p = quad;` plus volatile views, which is crutch
debt. The OT link also needs a pointer-to-integer mask (`(u32)prim &
0xFFFFFF`), the same debt the siblings carry.
