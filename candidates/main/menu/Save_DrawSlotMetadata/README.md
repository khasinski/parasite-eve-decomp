# Save_DrawSlotMetadata (0x8003495C, 0x484 bytes, main.yaml 0x2515C, TU menu/misc23)

Typed plain-C draft replacing the old misc23.c byte-offset version for this
function (misc23.c still holds the other two functions of the old draft).
Score: **lev 28** (retail 289 words, mine 287 words). No pins, barriers,
volatile, aliases or integer casts.

Setup that already matches:

- cc1 stays at -G0 and maspsx gets `-G1`, with a tentative definition of
  `g_MenuActiveMode` in the unit: only that common byte becomes gp-relative,
  while the prompt word, its low byte and the textbox entry stay absolute
  (`lbu %lo(D_8009D1AC)` with no shared `la`).
- The low byte of the prompt word is its own symbol, `g_SavePromptTimer`
  (linker script, same address as D_8009D1AC); a union or a `(u8 *)&` view
  forces the address into a register.
- Masks are `~0x300`; `index != 0 ? 0x14 : 0x61`; the prompt text choice is
  `if (prompt != 0) { if (prompt == phase) B; } else A;`.
- Types: `SaveSlotSummary` (fields 0x10 and 0x88 copied into the colour table)
  and the message tables as `u8 [][N]` arrays in save_slot_metadata.h.

Remaining differences (lev 28):

1. Retail keeps the byte store `g_SavePromptTimer = 0x4B` (and the textbox
   state store in phase 1) ahead of the next `D_8009D1AC` load, and reloads
   the word after the byte store. With two symbols GCC sees no conflict, so
   the load is hoisted / the stored value is reused (about 12 words). Retail
   therefore addressed the byte through the same symbol as the word; a
   volatile word does not reproduce it either.
2. The early `state` lives in v1 in retail and in a0 here, a knock-on of 1
   (about 16 words of register names).
