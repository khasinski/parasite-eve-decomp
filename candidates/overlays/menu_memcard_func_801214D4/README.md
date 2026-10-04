# menu_memcard func_801214D4 (0x7D4, 0x1F0 bytes): video slice upload

Yaml: `[0x7D4, asm, func_menu_memcard_0007D4]`. Twin of the matched
Memcard_UploadVideoSlice (0x95D8), whose source still needs pins, barriers
and a volatile.

- `Memcard_UploadVideoSlice.inc`: older draft written against the twin's
  symbols.
- `Memcard_UploadVideoSlice_801214D4.c` (2026-10-04): clean draft against the
  existing `VideoDisplay D_801228CC` record in menu_memcard_video.h, with a
  `volatile u8 *selector` (retail reads the selector twice, `lbu s0` and
  `lbu v1` from the same register, so the re-read is real). lev 46 (120 vs
  124 words). Differences: retail copies the slice rectangle through
  `la a1` but reads rect.x / rect.w / region through absolute symbols
  (D_801228F4, D_801228F8, D_801228F2), keeps &selector in a3 for the reads,
  the store and the `buffers[next]` load (`lw a0,-8(v0)`), and reads the
  region after the selector store; the struct form lets cse reuse the copy
  address for the field reads instead.
