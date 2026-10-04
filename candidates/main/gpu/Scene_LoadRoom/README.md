# Scene_LoadRoom (0x8006B4F8, 0x870 bytes, main.yaml 0x5BCF8)

Typed plain-C draft, stock GCC 2.7.2 + maspsx. No pins, barriers, volatile
or integer casts. Three `goto` CD retry restarts (recorded debt when it
lands). Score: **lev 2** (retail 540 words, mine 540 words).

Small-data flags: `/* CC1_FLAGS: -G1 */` + `/* MASPSX_FLAGS: -G1 */`. Checked
against the approved -G8/-G4 split: the PE.IMG base is now read as
`g_GameState.pe_image_base_lba` (0x800B0DD8 is field 0x100 of g_GameState),
which stays absolute under -G4, but `g_PmCmdHandlerTable` (a 4-byte pointer
at 0x800942E0, outside the gp window) becomes gp-relative under any maspsx
limit of 4 or more. Retail reads it absolutely while the 1-byte room
letter `D_8009CDC8` is gp-relative, so a small-data limit below 4 is needed
(-G1, -G2 and -G3 all give the same code; cc1 -G8 with maspsx -G1 is also
identical).

Types live in `include/pe1/scene_room.h` (room directory, 12/8-byte records,
stream records, sector ranges); `g_GameState` gained `room_type` (0x08) and
`texture_load_scratch` (0x168), and its bank slot tables are pointer typed.

What matches now (new in this round marked *):

- * `ready` in s1 and `i` in s2: the CD read loops assign their result to
  `ready` (`while ((ready = CdRom_ReadSectorsFromLba(...)) == -1)`). Combine
  folds the compare onto v0, but flow already counted the extra references,
  which lifts `ready` (global-alloc priority) above `i`.
- * Frame 0x80: every `for` loop whose entry test folds from `0 < n` to
  `n != 0` leaves a dead `sltu` pseudo behind a combine USE, and each such
  pseudo gets an 8-byte stack slot. Retail has one fewer; writing one loop
  (bankRoots) as `i = 0; if (n) do { ... } while (++i < n);` removes it and
  gives retail's 0x80 frame with identical code. Any of the seven later
  loops works; bankRoots is the first.
- * `flags = 0` after the memset call puts `sw s7` / `move s7,zero` where
  retail has them.
- * Sample bank compare: `u16 bank = stream[i].bank.value;` read first gives
  retail's `lhu` before `lh`.
- * Samples table: `int key = sample[i].u.track.key;` with the test on the
  field reproduces retail's second register for the stored key (retail
  `move v1,v0`, here `andi v1,v0,0xffff`, 1 word).
- Control flow of the three read/poll phases, the TIM upload inside the
  poll loop, `&tim[i]`, `sltu` against `loaded` for the TIM loop entry, map
  number spilled to the stack, `SCENE_ROOM_PAYLOAD` ordering, `slot - 0x55`
  unfolded, stream records stored back as bytes, tail `(flags & 1) && bank`.

- * Typing (2026-10-04, round 2): `SceneAssetView` (header + byte view) and
  `SCENE_ASSET_AT` moved from scene_entity_textures.h to scene_assets.h;
  `Pe1GameState.loaded_scene_assets` and `texture_load_scratch` are
  `union SceneAssetView *` (the two Asset_* users take `->header`), and
  `SCENE_ROOM_PAYLOAD(view, record)` is `SCENE_ASSET_AT(view, offset)`, so
  there is no byte-pointer arithmetic or cast in this unit. The inline
  `SceneAsset_ResolveOffset` for the payload costs lev 25 (it reorders the
  slot address), so the macro form stays.
- * Script table without the `(void **)` cast: the two tables are written
  as separate branches (`PmCommand **command = &g_PmCmdHandlerTable[slot]`
  and `void **handler = &D_800E1044[entry]`), each with its own
  `if (*p == 0) *p = record[i].u.handler;`. jump2 cross-jumps the two
  identical tails into retail's shared one. `index = slot;` before the
  range test gives retail's `move a0,v1` copy for the `>= 0x55` branch, and
  a block-local `entry = index - 0x55` keeps the subtract unfolded.

Remaining differences (lev 2):

1. Table branch: `addu v1,v0,v1` (table + scaled slot) where retail has
   `addu v1,v1,v0` (scaled slot + table). Tried `slot + table`,
   `table + slot`, a local table pointer, `command += slot` (lev 9,
   swaps the slot/copy registers), `(int)slot`, `slot & 0xFF`, the field
   re-read as index.
2. Samples table: `andi v1,v0,0xffff` instead of `move v1,v0` (the key copy
   is a zero_extend of the HImode load rather than an SImode copy).
   `(int)` cast on the test, `& 1U`, `unsigned int`/`u16`/`short` key types
   did not give a plain move.
3. Typing debt still in the unit: `state->bank_asset_table` (a `Pe1U32`)
   is assigned a resolved pointer, as Akao_LoadVoiceBankAlt does.
