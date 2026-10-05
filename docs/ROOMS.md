# Room and scene overlays: numbering and place names

This note records what the retail data says about the `room_mNNN` and
`scene_eNN` overlays: what the number is, where each map is in New York, and
how far the numbers follow the story. It also defines the
`room_mNNN_<place>` naming scheme and lists the entries that still need a
human decision.

## Summary

- `NNN` in `room_mNNN` is the game's own map number, not an index chosen by
  this project. The engine builds a scene token from a room letter and the
  number (`Str_EncodeBase32`), reads the digits back with
  `Str_ParseMapNumber`, and uses `number - 1` as the row of the room sector
  table at `0x80093378` (`Scene_LoadRoom`). The special-scene table at
  `0x80094048` is the same table continued: `scene_eNN` is row `409 + NN`,
  that is map number `410 + NN`.
- The retail data contains a per-map location table that the save screen
  uses. It gives an authoritative place label for 383 of the 437 map
  numbers. Every place name in this note comes from that table, checked
  against the dialog banks embedded in each map. Nothing is invented.
- Verdict on "higher numbers are later in the story": **true for the main
  story block (maps 1-287) and for the post-game block, false for a block of
  late additions (maps 319-381).** Measured on the 161 room overlays that
  have a place (all but m350):
  - Kendall tau = 0.83, Spearman rho = 0.88 between map number and story
    position, over all of them;
  - 0 discordant pairs out of 9,373 (tau = 1.00) once the 13 overlays of the
    late-addition block are set aside and the second NYPD visit is treated
    as its own story segment;
  - the 13 late-addition overlays (m319-m380) are the only exceptions; the
    maps of that block run backwards (tau = -0.23 over its 49 labelled maps).

## Evidence

### The save-location table

`Scene_LoadSceneData` reads PE.IMG sectors 72-86 (the pair at `0x800930DC`)
into `0x800A8028`, a global data pack. Its header is a list of offsets from
the start of the pack. `Save_FormatTitle` calls
`Tbl_FindNthNonEmpty(Save_GetCurrentMapNumber())`, which

1. takes header word 9 (`0x6FA8`): a byte array indexed by map number, 440
   entries;
2. takes header word 11 (`0x731C`): NUL-separated Shift-JIS labels;
3. returns the label whose index is the byte for the current map (0 means no
   label).

The labels, in table order:

| Id | Label | Place | Slug |
|---|---|---|---|
| 1 | Theater | Carnegie Hall | `theater` |
| 2 | Sewer | sewer under the theater | `sewer` |
| 3 | N.Y.P.D. | NYPD 28th Precinct | `nypd` |
| 4 | Central Park | Central Park | `central_park` |
| 5 | Soho | Soho | `soho` |
| 6 | Hospital | St. Francis Hospital | `hospital` |
| 7 | Chinatown | Chinatown | `chinatown` |
| 8 | Sewer2 | second sewer | `sewer2` |
| 9 | Subway | subway | `subway` |
| 10 | Museum | American Museum of Natural History | `museum` |
| 11 | Warehouse | Pier No. 4 warehouse | `warehouse` |
| 12 | Carrier | aircraft carrier | `carrier` |
| 13 | Liberty | Statue of Liberty | `liberty` |
| 14 | Cruiser | cruiser | `cruiser` |
| 15 | Clear Data | title of a clear-game save (looked up as index -1) | - |

The long forms in the "Place" column come from the centred location captions
in the engine image at PE.IMG sector 1975 (ids 1-10 there: Carnegie Hall,
NYPD 28th Precinct, American Museum of Natural History, Central Park, Soho,
St. Francis Hospital, Chinatown, Pier No. 4 Warehouse, Subway, Chrysler
Building). That caption list has no entry for the sewers, the carrier, the
statue or the cruiser, and the save table has no entry for the Chrysler
Building.

### Dialog banks

Each map's three sector ranges carry the dialog bank of its chapter in the
game's glyph code (byte = ASCII - 0x31). Counting the occurrences of 22
character and place words per map and grouping equal counts gives 19
clusters. Apart from one cluster of maps without any of the words, every
cluster is pure or nearly pure with respect to the save label: for example
all 52 maps of the hospital cluster are labelled Hospital, all 69 of the main
museum cluster are labelled Museum, and the sewer2 and subway maps share one
bank. That agreement is the cross-check behind the confidence levels below.

One cluster (76 maps: 288-318, 382-424 and 434-437) has no save label at
all. Its bank contains an elevator list running from the 1st to the 70th
floor. A 70-floor building with no save label that is not part of the main
story matches the post-game Chrysler Building, which the caption list names.
This is the only place name not read directly from the save table, so it is
rated medium.

The `# text-bank:` comments that the overlay configs carried before this note
came from an earlier keyword heuristic. 62 of the 162 room comments
contradicted the save table (for example m100-m123 were labelled "theater
blaze aftermath" but are NYPD maps, and m197-m246 were labelled "Soho / St.
Francis Hospital" but are museum maps). They are replaced by `# area:`
comments that quote the save table.

### Map numbers by label

Runs of equal labels over all 437 map numbers, with the configured overlays
in each run (`m` = `room_m`, `e` = `scene_e`):

| Maps | Label (id) | Configured overlays |
|---|---|---|
| 1-25 | Theater (1) | m005, m013, m014, m016, m017, m018, m021, m022, m023 |
| 26-34 | Sewer (2) | m027, m028, m030, m032, m034 |
| 35-40 | none (0) | - |
| 41-56 | N.Y.P.D. (3) | m045, m049, m050 |
| 57-89 | Central Park (4) | m059, m061, m062, m063, m065, m066, m067, m075, m078, m080, m081, m082, m083, m085, m086, m087, m089 |
| 90-97 | Soho (5) | m090 |
| 98-124 | N.Y.P.D. (3) | m100, m102, m104, m107, m110, m111, m113, m114, m115, m116, m118, m120, m121, m122, m123 |
| 125-160 | Hospital (6) | m126+1, m129+1, m137, m139+1, m140, m141, m145, m146, m147, m149+1, m151, m152, m153, m154, m156+2, m159 |
| 161-165 | Chinatown (7) | m162, m163 |
| 166-181 | Sewer2 (8) | m167, m168, m169, m173, m174, m178 |
| 182-190 | Subway (9) | m186, m187, m188 |
| 191-259 | Museum (10) | m197, m198, m199, m201, m203, m205, m206, m207, m210, m214, m217, m218, m219, m220, m221, m223, m224, m226, m227, m229, m231, m232, m233, m234, m242, m243, m245, m246, m256 |
| 260-270 | Warehouse (11) | m263, m265, m269 |
| 271 | Carrier (12) | - |
| 272-273 | Liberty (13) | m272, m273 |
| 274-287 | Cruiser (14) | - |
| 288-318 | none (0) | m291+2, m295, m298, m299, m308, m309, m310, m311, m318 |
| 319-332 | Hospital (6) | m319, m324, m326+1, m327, m328, m332 |
| 333 | Warehouse (11) | m333 |
| 334 | Sewer (2) | - |
| 335 | none (0) | - |
| 336-338 | N.Y.P.D. (3) | - |
| 339 | Hospital (6) | - |
| 340-346 | Museum (10) | m344 |
| 347 | none (0) | - |
| 348 | Sewer (2) | m348 |
| 349 | Cruiser (14) | m349 |
| 350-355 | none (0) | m350 |
| 356-357 | N.Y.P.D. (3) | - |
| 358 | Central Park (4) | m358 |
| 359 | Theater (1) | - |
| 360-363 | none (0) | - |
| 364 | Museum (10) | - |
| 365-366 | none (0) | - |
| 367-368 | Theater (1) | - |
| 369-370 | N.Y.P.D. (3) | - |
| 371 | Museum (10) | - |
| 372-373 | Theater (1) | - |
| 374-375 | Central Park (4) | m374 |
| 376 | N.Y.P.D. (3) | - |
| 377-378 | Theater (1) | - |
| 379 | Museum (10) | - |
| 380-381 | Hospital (6) | m380+2 |
| 382-424 | none (0) | m383-m410, e01-e14 |
| 425-430 | Cruiser (14) | e18, e19, e20 |
| 431 | Theater (1) | (e21, not configured) |
| 432 | Liberty (13) | e22 |
| 433 | Sewer2 (8) | (e23, not configured) |
| 434-437 | none (0) | e24, e25, e26, e27 |

The `scene_eNN` rows fit the `410 + NN` rule: e18-e20 carry the cruiser bank
and the Cruiser label, e22 the statue bank and the Liberty label, and the
unconfigured set 21 (map 431) carries the Day 1 theater bank and label 1.

### Story order

Blocks of the map numbering, with the story position as publicly known for
the game (days are the in-game days; the story-day state the engine keeps at
`g_GameState+0x0C` takes values 1-8, but this note does not yet tie a day to
each map from the scripts):

| Maps | Places | Story position | Confidence |
|---|---|---|---|
| 1-34 | theater, sewer | Day 1 | high |
| 35-56 | (cutscene maps), NYPD | Days 1-2 | medium |
| 57-89 | Central Park | Day 2 | high |
| 90-160 | Soho, NYPD revisit, hospital | Day 3 | high for Soho and the hospital, medium for the NYPD revisit |
| 161-190 | Chinatown, sewer2, subway | Day 4 | medium |
| 191-270 | museum, warehouse | Days 4-5 | medium |
| 271-287 | carrier, statue, cruiser | final day | high |
| 288-318 | Chrysler Building | post-game | medium |
| 319-381 | every earlier place again | late additions, no story order | high (from the labels) |
| 382-424, 434-437 | Chrysler Building (and its special scenes) | post-game | medium |
| 425-433 | cruiser, theater, statue, sewer2 special scenes | as their places | high |

The main block lists every place once, in story order, with one deliberate
exception: the NYPD appears twice (41-56 and 98-124), and the second run sits
exactly where the story returns to the precinct. The numbering therefore
looks like a level list written in story order and then extended: first the
post-game building, then extra rooms for places that already existed
(319-381) appended at the end, then more post-game floors.

### Numbers

Story position used for the measurement: the save label id, which is in
story order (1 theater ... 14 cruiser), with the Chrysler Building after the
cruiser. Pairs with equal positions are ignored.

| Population | Items | Concordant | Discordant | Kendall tau | Spearman rho |
|---|---|---|---|---|---|
| room overlays with a place | 161 | 10,165 | 960 | 0.83 | 0.88 |
| the same, NYPD revisit as its own segment | 161 | 10,450 | 720 | 0.87 | 0.89 |
| room overlays outside 319-381, NYPD revisit as its own segment | 148 | 9,373 | 0 | 1.00 | 1.00 |
| room and scene overlays with a place | 183 | 12,903 | 1,168 | 0.83 | 0.89 |
| all labelled maps 1-287 | 281 | 33,149 | 1,107 | 0.94 | 0.96 |
| all labelled or Chrysler maps 1-410 | 390 | 56,714 | 10,090 | 0.70 | 0.72 |
| all labelled maps 319-381 | 49 | 365 | 577 | -0.23 | -0.31 |

All the discordance in the main block comes from the NYPD revisit, which is
a real story revisit, not a numbering inversion.

## Naming scheme

Overlays that have a place get a suffix: `room_mNNN_<place>` (the `+N`
variants keep their marker: `room_m126+1_hospital`).

- `NNN` stays: it is the game's map number and the key that links the
  overlay to the room table, the save table and the retail asset name.
- `<place>` is the slug of the save-table label (lowercase ASCII, snake
  case, punctuation dropped), plus `chrysler` for the unlabelled post-game
  building. A slug is a property of the map number, so two overlays with the
  same label always get the same slug, and the slug never changes unless the
  evidence does.
- An overlay with no evidenced place keeps its plain name. If one is ever
  needed, the reserved form is `unknown_a00` (area id 0 = no save label),
  never a guess.
- The retail asset name does not change: the configuration keeps
  `target_path: original/USA/overlays/room_mNNN.bin`. Only the source,
  configuration, linker and build names carry the suffix.

### What a rename touches

The overlay name is the config file stem, and every tool derives the source,
asm, linker and build paths from it (`parallel_overlay_make.py`,
`gen_expected.py`, `objdiff_config.py`, `check_source_policy.py` and the
Makefile). The retail binary is the only thing addressed by `target_path`
instead, and the Makefile reads it from the config too, so the asset files
in `original/USA/overlays/` and in the CI asset checkout keep their names.
A rename of `room_mNNN` to `room_mNNN_<place>`:

- moves `configs/USA/overlays/<name>.yaml` and `sym.<name>.txt`, and
  replaces the name everywhere in the config except `target_path` and the
  `dd ... of=` extraction comment;
- moves `src/overlays/<name>/` and, when present, `candidates/overlays/<name>/`;
- rewrites `../<name>/` includes in other overlays and `overlays/<name>/`
  paths in tests and candidate scripts.

Generated `asm/`, `linkers/` and `build/` directories follow on the next
split; the old ones can be deleted. Headers under `include/pe1/` that carry a
room number in their file name (`room_m089_trail.h`) and prose in other docs
keep the number, which still identifies the overlay.

All 161 room overlays with a place of medium or high confidence are renamed;
room_m350 keeps its name. The scene overlays keep theirs.

### Overlays by place

| Slug | Confidence | Count | Overlays |
|---|---|---|---|
| `theater` | high | 9 | m005, m013, m014, m016, m017, m018, m021, m022, m023 |
| `sewer` | high | 5 | m027, m028, m030, m032, m034 |
| `nypd` | high | 16 | m045, m100, m102, m104, m107, m110, m111, m113, m114, m115, m116, m118, m120, m121, m122, m123 |
| `nypd` | medium | 2 | m049, m050 |
| `central_park` | high | 19 | m059, m061, m062, m063, m065, m066, m067, m075, m078, m080, m081, m082, m083, m085, m086, m087, m089, m358, m374 |
| `soho` | high | 1 | m090 |
| `hospital` | high | 22 | m126+1, m129+1, m137, m139+1, m140, m141, m145, m146, m147, m149+1, m151, m152, m153, m154, m156+2, m159, m324, m326+1, m327, m328, m332, m380+2 |
| `hospital` | medium | 1 | m319 |
| `chinatown` | high | 2 | m162, m163 |
| `sewer2` | high | 6 | m167, m168, m169, m173, m174, m178 |
| `subway` | high | 3 | m186, m187, m188 |
| `museum` | high | 30 | m197, m198, m199, m201, m203, m205, m206, m207, m210, m214, m217, m218, m219, m220, m221, m223, m224, m226, m227, m229, m231, m232, m233, m234, m242, m243, m245, m246, m256, m344 |
| `warehouse` | high | 4 | m263, m265, m269, m333 |
| `liberty` | high | 2 | m272, m273 |
| `sewer` | medium | 1 | m348 |
| `cruiser` | medium | 1 | m349 |
| `chrysler` | medium | 37 | m291+2, m295, m298, m299, m308-m311, m318, m383-m410 |
| (none) | low | 1 | m350 |

Confidence rules: **high** = save label present and the dialog bank agrees
(or the bank is empty and the map sits inside a run of at least three maps
with the same label); **medium** = save label present but the bank belongs
to another place, or no label but the Chrysler floor bank; **low** = neither.

Scene overlays are not renamed. Their places, for reference: e18-e20
cruiser, e22 liberty (high); e01-e14 and e24-e27 Chrysler bank (medium).

## Entries to confirm

1. **Chrysler Building (37 rooms, 18 scenes).** No save label; named from the
   70-floor elevator bank and the caption list. If the post-game building is
   something else, these become plain names again.
2. **m049, m050 (`nypd`).** Labelled N.Y.P.D. but carry the Day 1 theater
   bank; probably precinct rooms used at the end of Day 1.
3. **m319 (`hospital`).** Labelled Hospital, theater bank.
4. **m348 (`sewer`).** Labelled Sewer, but alone in the late-addition block
   and its bank has none of the keywords.
5. **m349 (`cruiser`).** Labelled Cruiser, museum-side bank.
6. **m350 (not renamed).** No save label; its bank is the cruiser bank, so it
   is probably a final-day scene, but the evidence is not enough for a name.
7. **Slug style.** `theater`, `nypd`, `hospital`, `warehouse`, `liberty`
   and `cruiser` are the game's own words. The longer forms
   (`carnegie_hall`, `precinct`, `st_francis_hospital`, `pier4_warehouse`,
   `statue_of_liberty`) are equally supported by the caption list if
   preferred; a switch is a mechanical rename.

## Reproducing

All of the above is read from retail data with short scripts:

- room table rows: 8-byte records at `0x80093378` in `SLUS_006.62`
  (`u32 start; scratch:8, texture:12, room:12` sectors, relative to the
  start of PE.IMG); map `N` is row `N - 1`;
- save table: PE.IMG sector 72, header word 9 (map byte table) and word 11
  (labels);
- dialog-bank clusters: the word counts over each map's full sector range,
  with each word encoded as `ord(c) - 0x31` (space = `0x0F`).
