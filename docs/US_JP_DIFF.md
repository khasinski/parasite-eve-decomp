# US and JP executable comparison

`tools/scripts/us_jp_diff.py <us main.exe> <jp SLPS_012.30> <nm -S -n output> <out.csv>`
compares every USA function against the JP executable (`disc/jp/extracted/SLPS_012.30`,
SLPS-012.30) and writes a per-function CSV (class and JP address). The CSV is an analysis
artefact, not part of the build. `tools/scripts/jp_func_diff.py` diffs one function pair.

Both executables are 2,025,472 bytes with the same load address (0x80010000) and the
same size header, but the code is not laid out identically.

| Class | Functions | Meaning |
| --- | ---: | --- |
| identical | 97 | same bytes at the same address |
| relocated | 243 | same address, equal once jump targets and lui/addiu/load-store immediates are masked |
| shifted | 2083 | same code found at another JP address (column `jp_address`) |
| different | 90 | no masked match anywhere in JP (changed or JP-only code) |

The first 0x11000 bytes of text (up to Pad_GetMenuPressedBitOrDisabled) are identical.
After that JP runs 4 to 1200 bytes below the USA addresses, in 383 runs of constant
offset, so the JP image is the USA code with small insertions and deletions rather
than a different program. Two single functions sit 17.7 KB away
(Menu_DrawBlendColorChannelList, Menu_DrawBlendColorOptionList).

## JP target (`make VERSION=JP`)

JP is a separate target with its own tree, like USA: `configs/JP/`, `linkers/JP/`,
`assets/JP/`, `build/JP/` and `src-jp/` (hand-maintained, `src-jp/main`, `src-jp/overlays`).
Nothing is generated at build time and the C is not shared with `src/`; a fix in one tree
does not automatically apply to the other, because the same C has to produce different
bytes in each.

`make VERSION=JP split build check` reassembles `SLPS_012.30` byte for byte (sha1
411bd943...). `configs/JP/main.yaml` lists all 369 code units of `main` (335 `c` translation
units and 34 `s` original-assembler units — 128 game units and 241 PsyQ library units total,
matching the USA translation-unit layout 1-to-1 with zero splat `asm` subsegments remaining),
alongside the same named `rodata`, `.data`, `.sdata`, `linker_offset` (`main_sbss`,
`main_scommon`), `.bss` (`script/sfx_queue`), `field_engine`, and `overlay_windows`
subsegments as `configs/USA/main.yaml`. `configs/JP/sym.main.txt` and
`configs/JP/sym.main_only.txt` hold the JP addresses of all 3,507 symbols, while
`linkers/JP/undefined_syms_manual.txt` holds only SPU hardware registers, LIBCARD kernel patch
addresses, and symbolic aliases (matching `linkers/USA/undefined_syms_manual.txt`). The target
exe is copied to `assets/JP/main.exe` (ignored, like `assets/USA`).

`tools/scripts/jp_asm_fixup.py` rewrites `sd`/`ld`/`sdc2`... lines that splat mis-decodes
inside data to `.word`, because GAS expands them and shifts every later address, and
`tools/scripts/ld_postpass.py` post-processes `linkers/JP/main.ld` and `asm/JP/main/header.s`
for shiftable `.sdata`/`.sbss`/COMMON layout and `linkers/JP/overlay_windows.txt` inclusion.

Every translation unit in `src-jp/main/` is now unified into a single `.c` file matching its
USA counterpart in `src/main/`, with JP-specific function changes, debug helpers, and Shift-JIS
text handling integrated directly in place.

## JP overlays

`configs/JP/overlays/*.yaml` are the USA overlay configs with the JP `PE.IMG` offsets and
sha1, window vrams moved and symbols retargeted. `tools/scripts/jp_overlay_check.py --all`
builds and checks them: all 190 match byte for byte (152 rooms, 32 battle scenes, and all 6
system overlays: `boot_display`, `fx_common`, `fx_common_sector17`, `fx_field`, `mdec_diag`,
and `menu_memcard`). `menu_memcard` has the JP-only changes (the logo fade function is gone,
`MenuCard_RunListScreen` and the page-flip/image-node constants differ, the video player data
sits 0x23BC lower); `boot_display` needed JP symbol offsets and entry names. JP moved the
overlay windows: mdec_diag and menu_memcard by -0x540, boot_display by -0x400, the rest stay
(`linkers/JP/overlay_windows.txt`).

## JP match report

The USA report pipeline runs for JP with `PE_VERSION=JP` (`gen_expected.py`, `objdiff_config.py`;
`SRC` is `src-jp` there). Once all overlays are built (`tools/scripts/jp_overlay_check.py --all`),
the full report can be generated across the entire executable and all 190 overlays:

    PE_VERSION=JP make VERSION=JP report

| | functions matched | code bytes matched |
| --- | ---: | ---: |
| Main: game code | 1806 of 1806 (100.0%) | 555,604 of 555,604 (100.0%) |
| Main: PsyQ libraries | 548 of 548 (100.0%) | 83,928 of 83,928 (100.0%) |
| Room overlays | 7929 of 7929 (100.0%) | 2,390,640 of 2,390,640 (100.0%) |
| Battle scene overlays | 1194 of 1194 (100.0%) | 407,388 of 407,388 (100.0%) |
| System overlays | 166 of 166 (100.0%) | 107,280 of 107,280 (100.0%) |
| **Total** | **11,643 of 11,643 (100.0%)** | **3,544,840 of 3,544,840 (100.0%)** |

USA is 100.0% of 11,646 functions (3,546,164 code bytes across all 190 overlays). Both USA and
JP achieve 100.0% function and code-byte match across all translation units (`main-game`,
`main-psyq`, `rooms`, `scenes`, and `system`), with 1,171 semantic-C units audited at 100%
consistency.

## JP-specific functions

All translation units whose JP build differs from USA are now fully decompiled and unified into
their canonical `src-jp/main/<subsystem>/<unit>.c` files (such as `field/actor_collision.c`,
`draw/textbox.c`, `menu/item_actions.c`, `save/game_data.c`, `menu/save_screens.c`,
`menu/status_panels.c`, `menu/equip_select.c`, `menu/skills.c`, `menu/main_menu.c`,
`menu/inventory.c`, `menu/lists.c`, `battle/setup.c`, `battle/turn.c`, `script/flow_opcodes.c`,
`psyq/libc/sprintf.c`, `psyq/libcd/sys_2.c`, `psyq/libcd/bios_4.c`, `psyq/libcard/end.c`,
`psyq/libds/dsfile.c`, and `psyq/libgte/patchgte.c`).

The field-effect engine (`fieldfx/object_script`, `collision`, `model`, `glow`, `burst`,
`task_list`, `sprite_effects`, `emitters`, `geometry`, `vector`, `angle`, and `sprite_render`:
324 functions across 12 units) is also built completely from C at JP offsets (USA offset minus
0x540), with `sprite_render` in a `field_engine` segment of its own as in the USA config (its
jump table sits before the engine's text).
