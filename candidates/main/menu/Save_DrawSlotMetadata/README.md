# Save_DrawSlotMetadata (0x8003495C, 0x484 bytes, main.yaml 0x2515C, TU menu/misc23)

Typed plain-C draft replacing the old misc23.c byte-offset version for this
function (misc23.c still holds the other two functions of the old draft).
Score: **lev 4** (retail 289 words, mine 289 words). No pins, barriers,
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

- Phase 1 stores the advanced word through `phase` (`phase = (next & ~0x300)
  | ...; word = phase;`), a multi-block variable, so the new word goes to
  global allocation instead of local-alloc's v1 (found by the permuter).

Remaining differences (lev 4), all in the phase 1 tail:

- The new word now lands in s0 (the register of `phase`) instead of retail's
  a0. With a block-local temporary it is built in v1 (lev 6), so
  `lui v1,0x200` cannot move above `sw v1, word` in sched2. sched1 orders
  both versions the same (store, flags load, constant: the constant is a
  birthing insn and gets LAUNCH priority), so the difference is register
  allocation. Tried: statement orders (flags first, flags between load and
  store, separate `flags` temp), operand orders of the `|`, `+` instead of
  `|`, `0xFFFFFCFF`, `(x & 0x300) >> 8`, folding the mask after the shift,
  direct double reads of the word, storing through `prompt` (lev 7), `next`
  (lev 8) or `state` (lev 6).
