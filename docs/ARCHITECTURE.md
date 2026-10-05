# Architecture

What the shipped program consists of and how this repository rebuilds it.
Addresses come from `configs/USA/main.yaml`, the overlay configs and the
symbol files; names are project names unless they are PsyQ names.

## Shipped binaries

Parasite Eve (USA, disc 1) boots `SLUS_006.62`, a PS-X EXE. Almost everything
else the game loads comes from one large archive on the disc, `PE.IMG`,
addressed by sector. The archive holds room and scene data, textures (TIM),
AKAO music and samples, and the overlays: code blocks that the executable
reads into fixed RAM windows on demand. Overlays are not relocated; each
block is linked for one address.

The repository rebuilds the executable and 190 overlay binaries and checks
each against the retail SHA-1.

## Main executable memory map

| Range | Contents |
| --- | --- |
| file `0x000..0x800` | PS-X EXE header; load address `0x80010000`, size `0x1EE000`, entry `__SN_ENTRY_POINT` (`0x80072534`) |
| `0x80010000..0x8001220C` | read-only data: switch tables and constants, in the order of their objects |
| `0x8001220C..0x800718D0` | game code: `main`, script interpreter, actors, battle, menus, items, rendering, 2D drawing |
| `0x800718D0..0x8008D7C0` | PsyQ libraries interleaved with game-side drivers that sit beside them: CD, memory card, pad, SPU/AKAO, GPU queues |
| `0x8008D7C0..0x800910A0` | AKAO sequencer: note timing and track commands |
| `0x800910A0..0x800C1CA0` | initialized data and small data; `$gp` = `0x8009CD70`; the main state block `g_GameState` is at `0x800B0CD8` |
| `0x800C1CA0..0x800E0060` | second code segment: the field effect engine (`src/main/engine`, effect pool, emitters, particles) with its own rodata |
| `0x800E0060..0x801FE000` | zero-filled image tail used as BSS, buffers and overlay windows |
| `0x801FFFF0` | initial stack top |

Entry is the PsyQ start-up (`psyq/libsn/SNMAIN.c`): it clears memory, sets
`$sp`, `$fp` and `$gp`, initializes the heap and calls `main`
(`src/main/boot/Boot_MainLoop.c`).

## Overlay windows

| Overlay | Link address | Count | What it is |
| --- | --- | --- | --- |
| `room_mNNN` | `0x8018EFE8` | 162 | code and data of one map (`m` number), section 3 of its PE.IMG record |
| `scene_eNN` | `0x8018EFE8` | 22 | special-scene sets: entry `NN` of the table at `0x80094048`, section 3 |
| `fx_field` | `0x8018EFE8` | 1 | field effect block |
| `fx_common` | `0x8018EFF0` | 1 | effect code shared by many rooms and scenes |
| `sys_reset` | `0x8010BCF8` | 1 | MDEC diagnostics and the PsyQ LIBPRESS VLC decoder with its tables |
| `menu_memcard` | `0x80120D00` | 1 | memory-card menu |
| `boot_display` | `0x80122FA4` | 1 | boot screens: text blocks, glyphs, display buffers |
| `render_clip` | `0x80170000` | 1 | one PE.IMG sector duplicating part of `fx_common`; data only |

Rooms, scene sets and the effect blocks share the `0x8018EFE8` window, so
only one of them is resident at a time. `boot_display`, `menu_memcard` and
`scene_e11..e14` contain fragments copied with their own link addresses; the
Makefile links those with `--no-check-sections` (`OVERLAY_VMA_OVERLAP`).

Overlays call into the executable directly by address. Each overlay is linked
separately against the executable's symbols; nothing is shared between
overlays at link time, so code common to several rooms exists once per room
(`src/overlays/room_lib/` holds the shared templates).

## How the game runs

- **Boot.** `main` opens `PE.IMG` (`OpenPeImage`, which also identifies the
  disc), initializes the game state and subsystems, loads scene data, then
  runs a dispatch loop keyed by a scene token; the default case runs one
  frame (`Boot_RunFrame`).
- **Frame.** `Boot_RunFrame` resets the GPU pipeline and ordering tables,
  handles field state transitions, updates the entity list and actors,
  renders, steps the AKAO voice table and presents the frame at VSync.
- **Rooms.** `Scene_LoadRoom` resolves a map number, streams the map's three
  PE.IMG ranges (scratch, textures, room payload), uploads their TIM lists
  and relocates the room directory into `g_GameState`: entity banks, script
  blocks, task slots and sound banks.
- **Scripts.** Actor and scene behaviour is bytecode: the interpreter in
  `src/main/task` runs per-actor command streams, and the process manager in
  `src/main/pm` schedules script batches in slots.
- **Battle.** The ATB battle system (`src/main/battle`, `aya`, `item`) runs in
  the main executable on top of the same actor and render code.
- **Menus.** The in-game menus (`src/main/menu`) are a widget tree with its
  own input queue; the memory-card screens are the `menu_memcard` overlay.
- **Sound.** Square's AKAO driver (`src/main/akao`) sequences music and
  effects on the SPU through the PsyQ LIBSPU routines.
- **Effects.** Visual effects run as callbacks in task pools: generic ones in
  the field effect engine segment, room- and scene-specific ones in the
  overlays.

## Build

```
configs/USA/main.yaml ──splat──> asm/, linkers/USA/main.ld
src/**/*.c ──cc.sh──> PsyQ cpp ─> GCC 2.7.2 cc1 ─> maspsx ─> GNU as ─> .o
.o + linker script + undefined_syms_manual.txt + generated aliases ──ld──> main.elf
main.elf ──objcopy──> build/USA/main.exe ──sha1──> retail
```

- `make split` regenerates `asm/` and linker scripts from the retail
  executable. All game code is C; the split assembly only provides data
  subsegments.
- `tools/scripts/cc.sh` compiles one file with the stock compiler. Markers at
  the top of a file select per-file options (`CC1_FLAGS`, `MASPSX_FLAGS`,
  `ASPSX_VERSION`, `GCC_VERSION: 2.8.1`, `ASSEMBLER: GNU`).
- `gen_undefined_addr_aliases.py` emits absolute addresses only for symbols no
  object defines; every such pin is shiftability debt.
- Overlays: `make overlay-check OVERLAY=<name>` extracts the slice from
  `PE.IMG` using the `extract locally with` comment in its config, splits it,
  compiles `src/overlays/<name>`, links it against the main symbols and
  compares the SHA-1. `make overlay-build-all` and `make overlay-check-all`
  cover all 190.
- `make report` disassembles the retail binaries afresh and produces the
  objdiff report published to decomp.dev; `make progress` renders the badges.

## Repository layout

| Path | Contents |
| --- | --- |
| `src/main/<subsystem>/` | executable source, one directory per subsystem (see [CONVENTIONS.md](CONVENTIONS.md)) |
| `src/main/psyq/lib*/` | PsyQ library objects, SDK names |
| `src/overlays/<overlay>/` | overlay source; `room_lib/` holds shared templates |
| `include/pe1/` | game headers; `include/pe1/akao/` the sound driver |
| `configs/USA/` | splat configs, symbols, relocations, SDK provenance, assembler evidence |
| `tools/scripts/` | build, check and report scripts; `tools/tests/` their tests |
| `docs/` | contracts (`CONVENTIONS.md`, `CODE_ORGANIZATION.md`, `ASM_AND_GTE_POLICY.md`), PsyQ notes, badges |
| `proposals/` | match and provenance evidence, mostly for PsyQ objects reproduced in C |

Not tracked: disc images, `PE.IMG`, the retail executable, the SDK and
compiler binaries, and everything generated (`asm/`, `linkers/`, `build/`).
