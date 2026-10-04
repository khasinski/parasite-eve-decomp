# menu_memcard func_80121C04 (0xF04) / func_801924F8 (0x9D08): open video

The two copies are byte-identical apart from relocations, so one template
(`Memcard_OpenVideo.inc`) lands both. First draft only; ~500 real diffs
(size 0x43C retail vs 0x444 mine).

Files: `Memcard_OpenVideo.inc`, `Memcard_OpenVideo_80121C04.c`, and
`menu_memcard_video.h.patch` (full 20-byte VideoEntry with path/enabled/
channel/end/x/y, the catalog/display/loc externs, prototypes; func_8010C0D8
takes a callback pointer; DsSearchFile declared returning s32 so the -1 test
needs no cast).

Notes for the next attempt:
- The 0x9D08 copy's `j` targets are linked at 0x801924F8 (Memcard_PlayVideo
  calls it as func_801924F8), so flipping it needs its own linkbase segment
  (vram 0x801924F8) plus a new segment for 0xA144.. at 0x8012AE88.
- Search loop: `found == 0 || found == -1` folds into `(found + 1) < 2`;
  separate `if (found == 0) continue; if (found != -1) break;` gives retail's
  beqz/beq pair.
- Retail does not hoist the `1` / `-1` constants out of the search loop but
  does hoist `1` out of both stream-poll loops (s0, s2): the loop shapes are
  not right yet.
- Retail returns 1 at the end, 0 for index >= 47; the s16 status (-1 timeout,
  0 frame) is tested after the 2000-poll wait.
