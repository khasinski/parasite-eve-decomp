# Conventions

The working contract for source in this repository. It describes the tree as
it is and the direction it is moving in. Evidence rules for placing code are
in [CODE_ORGANIZATION.md](CODE_ORGANIZATION.md); the assembler policy is in
[ASM_AND_GTE_POLICY.md](ASM_AND_GTE_POLICY.md). Where this file and an older
file disagree on naming or layout, this file wins.

Two principles decide most questions:

- **Fidelity to evidence, not to the pain.** Keep what the binary proves
  (addresses, object boundaries, ABI, PsyQ names). Everything the binary does
  not prove is ours to name and organize for the reader.
- **Only PsyQ names are original.** Sony SDK functions, objects and libraries
  keep their SDK names. Every other name in the tree is a project name chosen
  by responsibility, and may be improved when understanding improves.

## Naming

| Kind | Form | Examples |
| --- | --- | --- |
| Game function | `Prefix_VerbObject`, PascalCase after a subsystem prefix | `Scene_LoadRoom`, `Inv_InsertItem`, `CdRom_RestartSeek` |
| PsyQ function | exact SDK spelling | `SpuSetReverb`, `CdControl`, `RotMatrixYXZ`, `_card_auto` |
| Unknown function | `func_XXXXXXXX` (address, upper-case hex) | `func_80192F9C` |
| Global variable | `g_PascalCase` | `g_GameState`, `g_PlayTimeFrameCounter` |
| Unknown global | `D_XXXXXXXX` | `D_800966EC` |
| File-local (`static`) data or function | `s_PascalCase` data, `Prefix_Name` functions | |
| Type (struct, union, typedef) | PascalCase, no `_t`, no `struct` tag-only types | `Pe1GameState`, `FieldActor`, `AkaoTrack` |
| Struct member | `lowerCamelCase` for new code; existing `snake_case` members stay until the struct is reworked | `directoryOffset`, `pe_image_base_lba` |
| Enum constant / `#define` constant | `UPPER_SNAKE` with a subsystem prefix | `INV_CMD_GET_REMAINING_AMMO`, `STATE_FLAG_READY` |
| Macro | `UPPER_SNAKE` with a prefix that names its owner | `PE1_OFFSETOF`, `PSYQ_BIOS_TRAMPOLINE`, `GTE_LOAD_ROTATION_WINDOW` |
| Header | `include/pe1/<subsystem>[_topic].h`, lower snake | `cdrom.h`, `menu_widget.h`, `akao/track.h` |
| C file | named after the responsibility of the unit, same prefix as its functions | `Battle_EncounterInitializationFlow.c`, `Inv_ItemActions.c` |
| PsyQ C file | the SDK object name, SDK case | `psyq/libspu/s_ini.c`, `psyq/libcd/CdControl.c` |

Prefixes are fixed per subsystem (table below). Use the subsystem's prefix
even when the caller is elsewhere: a CD routine is `CdRom_*` wherever it is
called from. Legacy game-side spellings (`CD_`, `Cd_`, `SPU_`) are renamed to the
canonical prefix when a file is touched. SDK-internal names that look similar
(`CD_cw`, `CD_init`, `GPU_cw`, `_spu_init`) are PsyQ names and keep their
spelling.

**PsyQ file names.** A unit under `psyq/<library>/` is named after the SDK
object it reproduces, lower case (`INTR_VB` is `intr_vb.c`, the BIOS veneer
object `A50` is `a50.c`, `SSINIT_C` is `ssinit_c.c`). The object comes from
`configs/USA/psyq_provenance.json`; a 12-byte BIOS veneer that has no entry
there is named after the PsyQ signature object whose bytes it matches
exactly. When one object is still split over several units, because the
pieces need different compiler settings or carry declarations that do not
yet agree, the units are numbered in address order (`padcmd.c`,
`padcmd_2.c`, ...) and merged once they compile together. Every unit opens
with a one-line comment naming the library, the object (and part) and the
public symbols it defines. Functions and globals take the object's SDK
labels; private routines that only have generated `text_*` labels keep
project names.

`MEMMOVE` and `QSORT` are byte-identical in LIBC and LIBC2. The executable
links LIBC: its string and memory routines are the LIBC BIOS veneers
(`C21` strcat, `C42` memcpy, ...), not LIBC2's C bodies. Both objects
therefore live in `psyq/libc/`; there is no `psyq/libc2/`.

Placeholder names are migration state, not style: `func_`/`D_` symbols and
files such as `misc19.c`, `task5.c`, `cd_rom3.c` or `gap_*` that describe
position rather than responsibility. `make organization-check` ratchets their count; it
may go down, never up. A file name that ends in an address
(`RoomEffect_JointBeacon_8018FDC4.c`) is allowed only to tell apart several
instances of the same template inside one overlay.

Name by what the code does for the game (`Save_StartWriteSlot`), not by how
it does it (`Save_WriteBytesLoop`) and not by where it was found
(`Main_Func3`). Do not encode types, sizes or addresses in names that have
been identified.

## Subsystems in `src/main`

Each directory is a subsystem with one responsibility and one function
prefix. "Target" is where the files belong; moving them is a separate change
that keeps `make check` green and updates the manifest paths. Do not create a
directory for one file.

**PsyQ code under project names.** About 120 game-named units outside
`psyq/` lie inside SDK object ranges recorded in
`configs/USA/psyq_provenance.json`: about half of `gpu` (LIBGPU SYS and primitive
objects), the `CdRom_*` DS-system files in `cdrom` (LIBDS), the `CardObj_*`
files in `memcard` (LIBPAD/LIBCARD), most of `pad` (LIBAPI PAD/PATCH), the
64-bit helpers in `math` (LIBMATH), the `Spu_*` setters in `akao` (LIBSPU),
and `gte` (LIBGTE). They move to `psyq/<library>/` under their SDK names.
Confirm each with the provenance entry first: a unit can straddle an object
edge, as `akao/Spu_TransferAndLifecycle` did before LIBPAD WAITRC2's
`chkRC2wait` was split off it.

| Directory | Prefix | Responsibility | Target |
| --- | --- | --- | --- |
| `psyq/lib*` | SDK names | Sony PsyQ 4.0 libraries, one directory per SDK library | keep; receives the units above |
| `boot` | `Boot_` | `main`, start-up, subsystem init, the per-frame loop | keep |
| `sys` | `Sys_` | interrupt and BIOS glue, shutdown and reset | keep the game-side shutdown/state code; SDK parts and `setjmp.c` (LIBAPI veneer) to `psyq` |
| `cdrom` | `CdRom_` | game-side CD layer: sector reads from PE.IMG, retries, boot reads | keep the game part; LIBDS units to `psyq/libds`; `cd_rom*.c`, `misc9.c` renamed by content |
| `memcard` | `MemCard_` | memory-card access for saves | keep the game part; LIBPAD/LIBCARD units and BIOS veneers (`_card_*`, `InitCARD2`...) to `psyq` |
| `pad` | `Pad_` | controller input | mostly LIBAPI: move to `psyq/libapi`; what remains joins `boot` or `menu` input |
| `gpu` | `Gpu_`, `Draw_` | game 2D drawing: text, glyphs, wipes, number and bar widgets, packet pools | keep the game part (rename to `draw`, `Draw_` prefix); LIBGPU units to `psyq/libgpu`; `Scene_*` files to `scene` |
| `gte` | `Gte_` | none of its own | LIBGTE entries to `psyq/libgte`; dissolve |
| `math` | `Math_` | fixed-point and integer helpers | keep the game part; LIBMATH units to `psyq/libmath` |
| `render` | `Render_` | 3D object, room and sprite rendering, fades, camera | keep; `Scene_*Battle*` predicates to `scene` |
| `anim` | `Anim_` | skeletal animation decode and interpolation | keep |
| `entity` | `Entity_` | actor pools, movement, hierarchy, per-frame actor update | keep; `task.c` renamed by content |
| `obj` | `Obj_` | geometry-state entry slots | merge into `render` |
| `field` | `Field_`, `Geo_` | field map entries, collision and floor geometry | keep; `Geo_*` from `main` join it |
| `engine` | `FieldEng_`, `FieldAnim_` | the field effect engine in the separate `0x800C1CA0` segment: effect pool, emitters, particles | rename to `fieldfx` (`FieldFx_` prefix) |
| `scene` | `Scene_` | room and scene loading, story flags, scene dispatch | keep; receives `Scene_*` files from `gpu`, `main`, `render`, `time` |
| `overlay` | `Overlay_` | overlay tables and loading, overlay audio/texture slots | keep |
| `asset` | `Asset_` | PE.IMG asset tables, TIM upload | keep; `str.c` renamed by content |
| `task` | `Task_` | actor script interpreter: opcodes, expressions, node pool | rename to `script` (`Script_`); `Task_Gpu*` to `gpu`, `Task_ConvertSecondsToHMS` to `time` |
| `pm` | `Pm_` | process manager: slot table that runs script batches | keep |
| `event` | `Evt_` | game event delivery and deferred execution | keep; check `Evt_Deliver` against its LIBMATH overlap |
| `battle` | `Battle_`, `BattleCmd_` | ATB battle: turns, targeting, damage, enemy AI, status display | keep; `Inv_Build*List.c` to `item`, `Math_IntSqrt.c` to `math` |
| `aya` | `Aya_` | the player character: stats, level table, parasite-energy spells | keep; `misc7.c` to `battle`, `Draw_LookupGlyphMetrics` to `gpu` |
| `item` | `Inv_`, `Item_` | inventory, item table, equipment and its modifiers | keep; `Menu_*` files to `menu` |
| `menu` | `Menu_`, `MenuWidget_`, `MenuInput_` | in-game menus, widget tree, menu input queue | keep; `Inv_*`, `Save_*`, `Pad_*` files out to their owners |
| `save` | `Save_` | save-data layout, metadata, write flow | keep; `Sys_VSyncTimeout.c` to `sys` |
| `akao` | `Akao_`, `Seq_`, `Spu_` | Square's AKAO sound driver: sequencer, voices, SPU uploads | keep; LIBSPU units to `psyq/libspu` |
| `audio` | `Sfx_` | misnamed: both files are menu item-list code | dissolve into `menu` |
| `time` | `GameTime_` | play-time counters and timers | keep; receives `util/game_time2.c` |
| `table` | `Tbl_`, `Str_` | text and textbox lookup tables | rename to `text` (`Text_`) |
| `util` | `Util_` | vague | dissolve: `util.c` (inventory globals) to `item`, `game_time2.c` to `time`, `Util_ReturnTrue` to its caller's subsystem |
| `main` | mixed | splat's default bucket, not a subsystem | dissolve by prefix into the rows above; `gap_*` words go with the object they pad (most are SDK object tails) |

## Overlays

`src/overlays/<binary>/` holds the code of one overlay binary; the directory
name is the configured overlay name.

- **Rooms:** `room_mNNN` where `NNN` is the map number used by
  `Scene_LoadRoom`. Room overlays keep their numbers and gain a place-name
  suffix, for example `room_m005_carnegie_hall`. The suffix is lower snake case from the
  room's text bank (the `text-bank:` comment in its config). Renaming is a
  single change across config, `src/overlays`, symbol file and CI. The
  current `+1`/`+2` tails (`room_m126+1`) record an extraction delta, not an
  identity, and are dropped by that rename.
- **Scene sets:** `scene_eNN`, entry `NN` of the special-scene table at
  `0x80094048`. They get a suffix the same way once their scenes are named.
- **Shared overlays:** `fx_common`, `fx_field`, `menu_memcard`,
  `boot_display`: named by responsibility. Two names mislead and should be
  replaced in a config rename: `sys_reset` holds MDEC diagnostics and the
  PsyQ LIBPRESS VLC decoder (for example `mdec_vlc`), and `render_clip` is a
  data-only duplicate of one `fx_common` sector.
- **Shared overlay code:** `src/overlays/room_lib/` holds templates that many
  room and scene overlays instantiate (`*.inc` bodies, `*_family.h`
  declarations, `room_lib.h`). Each overlay still owns its instantiation file;
  identical code is not linked across overlays.
- **Function names inside overlays:** `RoomLib_` for `room_lib` templates,
  `RoomEffect_` for effect templates, `RoomMNNN_`/`SceneENN_` for code that
  belongs to one overlay, and the overlay's own prefix (target: `FxCommon_`,
  `FxField_`, `MenuCard_`) for the shared overlays.

## Headers

- `include/pe1/` holds the game's shared declarations; `include/pe1/akao/` the
  sound driver; PsyQ declarations live in `psyq_*.h`.
- One owner per declaration. A function prototype, global or type is
  declared in exactly one header: the header of the subsystem that defines
  it. Never repeat a prototype in a `.c` file; `make debt-check` forbids new
  file-scope `extern`s.
- No per-function headers. A header covers a subsystem or a coherent part of
  one (`menu_widget.h`, `field_effect_pool.h`). Existing one-function headers
  are folded into their subsystem header when touched.
- Module-private declarations shared by a directory live next to it
  (`src/overlays/room_lib/room_lib.h`, `src/main/psyq/libcd/bios_internal.h`).
  Declarations used by one file stay `static` in that file.
- Headers include what they use, have an include guard
  `PE1_<NAME>_H`, and contain no definitions that allocate storage.
- Moving a declaration must keep its exact width, signedness, qualifiers and
  prototype. Do not "fix" an uncertain ABI while centralizing it.

## Assembler

- All code is C. There are no `.s` files in `src/`, and none may be added;
  `make source-policy-check` rejects them.
- Inline assembler appears only through evidenced macros: GTE operations
  (`include/pe1/gte.h`, `gte_window.h`), the scratchpad stack switch
  (`boot_stack.h`), PsyQ assembler objects (`PSYQ_ASM_FUNCTION`,
  `PSYQ_BIOS_TRAMPOLINE`) and proven game assembler (`GAME_ASM_FUNCTION`).
- Every whole-function use is listed with address, size and evidence in
  `configs/USA/original_asm_evidence.json` (game) or
  `configs/USA/psyq_provenance.json` (SDK). No entry, no assembler.
- Register pins and empty barriers are compiler constraints, not
  instructions. They are allowed, counted by `make debt`, and removed whenever
  the match survives without them (`make drop-pins`, `make drop-barriers`).
  The `PE1_NOP*` scheduling slots (`psyq_nop.h`) are counted the same way.
- The compiler and MASPSX are stock. Per-file options use the markers
  `/* CC1_FLAGS: ... */`, `/* MASPSX_FLAGS: ... */`,
  `/* ASPSX_VERSION: ... */`, `/* GCC_VERSION: 2.8.1 */` and
  `/* ASSEMBLER: GNU */` at the top of the file.

## Shiftability

Code must be able to change size without breaking anything. The full rule set
is in [CODE_ORGANIZATION.md](CODE_ORGANIZATION.md#shiftability); in practice:

- Refer to code and data by symbol. Never write a program address in C: no
  `(T *)0x800XXXXX`, no integer that happens to equal an address, no table of
  addresses typed as integers.
- Fixed addresses are allowed only for hardware: I/O registers and the
  scratchpad, each behind a named macro (`FIELD_ENGINE_SCRATCH`,
  `BOOT_SCRATCHPAD_STACK_TOP`), and for the loader contract such as an
  overlay's load address.
- Data the program owns is defined in a translation unit. Absolute
  assignments in `linkers/USA/undefined_syms_manual.txt` and in the splat
  symbol files are migration state; each one removed is progress.
- Two views of the same data are two typed declarations of one symbol, not
  a second address.
- Disc data that embeds program addresses is a documented ABI: list it with
  its owner and keep those addresses pinned on purpose.

## Text-resident data

Data, tables and zero padding that sit inside code segments are typed as what
they are. Tables become `rodata` (or `data` for written words) subsegments in
text order (`linker_section_order: .text`, section `.code_data`); zero words
after `jr $ra` and its delay slot become `pad`. Neither earns code credit.

## Code style

- Four-space indentation, no tabs. Opening brace on the same line as the
  function or statement; `else` on the closing brace's line.
- Aim for 80 columns, hard limit 100. Break long argument lists one level
  deeper than the call.
- One declaration per line for locals that carry meaning; declare at the top
  of the block (C89, as GCC 2.7.2 requires).
- Use the project types from `common.h` (`u8`..`s32`) for fixed-width data,
  plain `int` for counters and indices.
- Use struct members, not byte offsets. `M2C_FIELD`, `RW8`-style accessors and
  raw `+ 0x1C` arithmetic are migration state.
- Comments say why: what the code is for in the game, which evidence fixes a
  boundary, why an odd construct is needed for the match. Do not narrate the
  code, record history ("was previously..."), or paste scores and run logs;
  those belong in commit messages.
- A construct that exists only to reproduce the original code generation
  (volatile view, unused local that keeps a stack slot, `goto` retry loop)
  gets a one-line comment saying so.

## A finished file

A file is done when all of these hold:

1. Its object matches retail inside a byte-identical linked binary.
2. Its name and every function and global in it have project or SDK names;
   no `func_`/`D_` remains for anything it defines.
3. It declares nothing at file scope that belongs to another unit; it
   includes the subsystem headers it needs and nothing else.
4. It uses typed structures and enums instead of offsets and magic numbers.
5. It contains no program addresses (shiftability), no unrecorded pins or
   barriers, and assembler only through evidenced macros.
6. It sits in the directory of its subsystem, and its comments explain the
   non-obvious.

A file that meets 1 but not the rest is matched, not finished. The
organization and debt ratchets measure the distance.
