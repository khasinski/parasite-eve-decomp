# Save_DrawSlotMetadata (0x8003495C, 0x484 bytes, main.yaml 0x2515C, TU menu/misc23)

Typed plain-C draft replacing the old misc23.c byte-offset version for this
function (misc23.c still holds the other two functions of the old draft).
Score: **lev 2** (retail 289 words, mine 289 words). No pins, barriers,
volatile, aliases, integer casts or extra linker symbols. Flags are the
approved `/* CC1_FLAGS: -G8 */` + `/* MASPSX_FLAGS: -G4 */` split.

Setup that already matches:

- `D_8009D1A8` is a record `{ SaveSlotSummary **summary; SavePromptState prompt; }`
  (save_slot_metadata.h) and the prompt word at 0x8009D1AC is a union of the
  word and its low byte (the frame counter). Because the counter and the word
  are one object, GCC reloads the word after the `timer = 0x4B` store and
  keeps the textbox state store ahead of the next word load, as retail does.
  The record is 8 bytes, above the -G4 limit, so both stay absolute while the
  1-byte `D_8009CE80` (save.h) is gp-relative. This replaced the old
  `g_SavePromptTimer` linker symbol and the tentative definition of
  `g_MenuActiveMode` (plain maspsx -G1).
- Phase 1 reloads the word into its own local (`next`); reusing `state` puts
  `state` in a0 for the whole function instead of retail's v1.
- Masks are `~0x300`; `index != 0 ? 0x14 : 0x61`; the prompt text choice is
  `if (prompt != 0) { if (prompt == phase) B; } else A;`.
- Phase 1 builds the advanced word in `value`, a function-scope int that
  also carries the cursor x argument of Menu_SetTextCursorRect
  (`value = next & ~0x300; value |= ...; word = value;`). The variable has
  several sets, so global allocation places it, and with no call crossed it
  lands in retail's a0 (the permuter suggested the shared temporary).
  Earlier forms: a block-local temp builds it in v1 (lev 6), storing through
  `phase` puts it in s0 (lev 4), a single `value = (...) | (...)` gives
  lev 3.

Remaining differences (lev 2):

- The `~0x300` mask constant is loaded into v1 (`li v1,-769; and a0,v0,v1`)
  where retail loads it into a0 (`li a0,-769; and a0,v0,a0`). The constant is
  a local pseudo and cannot tie to the global `value`. Tried
  `value = ~0x300; value &= next;` and `value = next & value;` (lev 7).
