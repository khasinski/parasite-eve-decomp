# menu_memcard func_80121C04 (0xF04) / func_801924F8 (0x9D08): open video

The two copies are byte-identical apart from relocations, so one template
(`Memcard_OpenVideo.inc`) lands both. Instance: `Memcard_OpenVideo_80121C04.c`
(the 0x9D08 instance needs no defines, the template defaults name it
Memcard_OpenVideo). Apply `menu_memcard_video.h.patch` first (full 20-byte
VideoEntry, CD prototypes, func_8010C0D8 takes a callback, PlayVideo calls
Memcard_OpenVideo / Memcard_StepVideo).

Status (2026-10-04, agent 5): NOT matchable under the current rules.

- Locals must be declared `VideoRect size; char name[32]; u8 result[8];` in
  that order (stack 0x18 / 0x20 / 0x40).
- Search loop: retail keeps `li v1,1` and `li v0,-1` inside a 17-insn call
  loop. Stock loop.c (threshold 29) always hoists them when the loop has
  loop notes: do/while, while (test rotated or duplicated), for(;;) with
  continue/break were all checked with -dL. The only shape that gives the
  retail loop byte for byte is a label + `goto` loop (no loop notes), which
  the parked .inc currently uses as a diagnostic. `goto` is banned for new
  code, so do not flip it.
- First stream-start loop: retail re-sets `li s0,1` in the back-edge delay
  slot, i.e. the `1` is hoisted out of the inner `while (ready != 1)` loop
  but NOT out of the outer retry loop. With any structured outer loop the
  hoisted reg is "halved" (count 36) with life 7 and threshold 29, so it is
  always moved. Same conclusion: retail used a goto retry here too (matches
  Boot_StartPlayback / Memcard_PlayVideo, which already use retry gotos).
- The stream recovery and frame loop are the same code as func_80122040,
  see candidates/overlays/menu_memcard_func_80122040/README.md for the
  shapes that fix them (comma setloc trick, do/while + if/else handler).
- Remaining after that: struct copy / display init scheduling, `regions[r]`
  wants `la; addu; lhu 0x16(v1)` instead of the lui-at array form, and
  `rect.w = wide ? 24 : 16` delay-slot order.
- Linking the 0x9D08 copy needs a new segment at 0x9D08 with vram
  0x801924F8 holding both 0x9D08 and 0xA144 (both link at the 0x801887F0
  base: j targets 0x801925AC.., 0x80192ABC..), plus a segment for
  [0xA458, func_8012B19C] at vram 0x8012B19C.
