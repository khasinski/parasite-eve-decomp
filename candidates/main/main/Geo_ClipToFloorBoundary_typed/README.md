# Geo_ClipToFloorBoundary (main 0xBDFC, 0xB68 bytes): parked at lev 413

First typed rewrite (2026-10-04). It replaces the old byte-offset draft,
which had the wrong structure (it called Geo_PointInTri and
Geo_ClipToFloorBoundarySub; retail calls only Math_FixedMul, eight times,
and itself). To build it, copy `floor_clip.h` to include/pe1/ and the .c to
src/main/main/. The file uses the approved split: CC1 -G8 (D_8009D1D8 is
read gp-relative) and maspsx -G4 (the 6-byte D_8009CD88 initialiser is read
through lui/addiu).

Score: lev.py main 0xBDFC 0xB68 = lev 413 (retail 730 words, mine 706).

What the function does (from the asm, verified against m2c):
- Copies the three -1 entries of D_8009CD88 into a local neighbour list.
- Two copies of the same walk, flat triangles (D_8009D1D8 == 0: vertex
  ids at words[1..3], edge ids at [4..6], neighbours at [7..9], 4-byte XZ
  vertices, 22-byte triangles) and sloped ones ([4..6], [7..9], [10..12],
  6-byte XYZ vertices, 28-byte triangles). The edge walk steps a halfword
  pointer from the triangle to triangle + 3, starting with the last vertex
  as the previous one.
- Each edge id is tested once per query (bit in D_8009DFB0). A bounding
  box test against radius D_8009CE2C, then the perpendicular distance
  (cross product / RampEdge.length), then the projection on the edge
  direction (two Math_FixedMul calls, measured from the endpoint with the
  lower vertex id) against the 16.16 edge length read as one word at
  offset 0 of the D_8009CE14 record (hence the FloorEdge union in
  floor_clip.h), with endpoint distance checks past either end.
- A touched edge with no neighbour (0xFFFF) or a neighbour whose first
  byte has bit 0x80 is published to D_8009CE0C..D_8009CE28 (max/min pairs)
  with the edge id in D_8009CE18, and 0 is returned. Otherwise the
  neighbour goes into the list, and after the three edges each listed
  neighbour is walked recursively.

Remaining differences: the frame (retail 0xC0, mine larger) and the
spill layout. Retail keeps px/pz, the triangle copy, the walk pointer, the
slot offset, the previous/next vertex values and the edge id in 8-byte
stack slots (sp+0x28..0x90, separate slots for the two halves), so the
variable split per half matters. The endpoint-distance branches are the
next thing to align against the m2c output.

## Frame work (lev 447 -> 413)

- edgeId, nextIndex, nextX and nextZ are function-scope variables shared
  by both halves. Retail spills them to one set of slots (sp+0x28..0x40).
  px/pz, the triangle copy, the walk pointer and the slot stay per half
  (retail uses separate slots, 0x48..0x90). The frame went from 232 to 208
  bytes. Retail's is 192.
- Queuing through a `s16 *queued` pointer instead of an index: -2.
- The remaining 16 bytes of frame: mine keeps two walk pointers per half
  (the `word` biv plus a reduced giv for the word[1]/[4]/[7] reads, both
  spilled). Retail has one walk pointer at 0x88, read with raw offsets,
  and the slot as a byte offset at 0x90 (the slot biv was eliminated into
  slot*2). This is the same loop.c reduction as in
  Render_SetupEntityPrims, so try making the reads go through a copy that
  is not replaceable.
