# Code organization contract

This repository is a long-lived reconstruction of the original program, not a
collection of independent decompilation puzzles. A byte match is necessary,
but it is not sufficient evidence that source boundaries, declarations, names,
or types are correct.

## Evidence order

Use evidence in this order when placing or combining source:

1. executable addresses, section layout, relocations, and manifest order;
2. known object or archive boundaries and matches to Psy-Q or AKAO libraries;
3. shared data, call relationships, and static state;
4. responsibility of the code and its callers;
5. existing filenames, which are hints rather than evidence.

Do not infer an original object boundary merely because splat emitted one
function or because one function is convenient to permute in isolation. Do not
merge non-contiguous code or cross a strong responsibility boundary merely to
reduce the file count.

## Directory taxonomy

- `src/main/<subsystem>/` contains code resident in `SLUS_006.62`.
- `src/overlays/<binary>/` contains code whose link address and lifetime belong
  to that overlay. Code shared by several overlays stays duplicated until
  binary evidence identifies a linked library or a reproducible generation
  rule.
- `src/overlays/room_lib/` holds such a linked library once. The room
  library (`RoomLib_ActorClasses.c`) is the same run of 53 functions, in the
  same order and with the same jump tables, in 125 room and scene overlays.
  Each overlay's manifest names the unit as a `../room_lib/RoomLib_ActorClasses`
  subsegment, and `make overlay-build` compiles it into that overlay's own
  build directory, so every overlay still links its own copy at its own
  address. Eight rooms link a sixth class right after it
  (`RoomLib_FloorWalkerClass.c`); whether that class was part of the same
  object cannot be told from the binary, so it is a unit of its own.
  Smaller effect objects shared by a few rooms live there the same way
  (`RoomEffect_DroppedFlare.c`, `RoomLib_TwelveElementEffect.c`, ...): each
  is a run of functions in the same order in every instance, with a rodata
  block (seed initialisers, jump tables) that is contiguous and in the same
  order too. Data the code reads from the room (packet templates, colour
  tables) stays in each room's manifest under one name per role, given in
  the room's symbol file. The methods of a room module's class (the run in
  front of each effect module: `RoomLib_PlantScript`, `RoomLib_Spawn6`,
  `RoomLib_RegisterDrawList`, `RoomLib_RegisterPairedTables`,
  `RoomLib_CloseTarget`) are one unit per function, because rooms replace
  single methods with their own; see `include/pe1/room_module.h`.
- `include/pe1/` owns game ABI, shared data structures, and declarations.
- `include/pe1/akao/` owns the AKAO command, queue, track, and SPU interfaces.
- hardware and SDK declarations belong in their subsystem header rather than
  being repeated in C files.

The subsystem list, the target directory for every current directory and the
naming rules are in [CONVENTIONS.md](CONVENTIONS.md).

Names such as `func_800...`, `misc`, `task7`, and `spu4` explicitly mean
"identity or module boundary not established". They are migration state, not a
stable naming convention. `make organization-check` prevents the address-name
and placeholder-name counts from increasing independently in main and overlays.

## Translation-unit rules

A merge or split is accepted only when all of the following hold:

- manifest order and addresses remain unchanged;
- the proposed unit is contiguous, unless independent object evidence proves
  otherwise;
- shared declarations and private state have a clear owner;
- `make check` remains byte-identical after a clean rebuild;
- configured overlays touched by the change pass `make overlay-check`;
- the evidence and any remaining uncertainty are recorded in the commit or in
  maintained documentation.

Large cohesive units are acceptable. Function count and aesthetics alone are
not reasons to invent source boundaries.

`src/` has a strict one-to-one relationship with committed manifest entries.
The one exception is a shared library unit under `src/overlays/room_lib/`:
many overlay manifests may list it, each at most once.
The check uses files present on disk as well as Git's index: an untracked C file
cannot enter a local build, and a manifest promotion cannot pass unless its
source will exist in a fresh clone.
Non-matching experiments are not tracked. A C file enters `src/` only in the
same change that configures its range as `c` and passes the binary check.

## Declaration ownership

New file-scope `extern` declarations in `.c` files are forbidden by the debt
gate. Put public or cross-unit declarations in the narrowest appropriate
header. Keep declarations private only when every use and definition are in a
single translation unit.

Existing declarations are migration debt tracked separately for main and
overlays. Moving a declaration must preserve its exact signedness, width,
qualifiers, and function prototype; do not "improve" an uncertain ABI while
centralizing it.

## Link ownership

Splat's `undefined_syms_auto` and `undefined_funcs_auto` files are address/name
catalogues, not linker inputs. A linker-script assignment overrides a genuine
definition from an object and can make promoted C appear to link while its
symbol remains pinned. The build generates `undefined_addr_aliases.main.txt`
from the linked objects and emits only symbols that remain unresolved.

Tentative COMMON globals remain pinned until their BSS/SBSS placement is
represented by the manifest and linker script. This is explicit layout debt;
ordinary text/data definitions become object-owned automatically and cannot be
silently shadowed by the complete splat catalogues.

## Review contract

Each change must state which claim it makes: binary match, semantic recovery,
typing, naming, organization, or crutch removal. A green byte check proves the
binary claim only. It does not by itself prove the other claims.

## Shiftability

A goal of the reconstruction is that the code can change size without
breaking anything: adding, removing or growing a function must not invalidate
another address. A matching build proves the bytes; shiftability proves the
source does not depend on where those bytes sit.

Rules for source that aims at shiftability:

- Code and data refer to each other by symbol. A fixed address in C is allowed
  only for hardware (I/O registers, the scratchpad) and for the contract with
  the disc loader, such as the address an overlay is loaded at.
- Data that the program owns is defined in a translation unit and placed by
  the linker. Absolute symbol assignments in linker or symbol files are
  migration state, not a design.
- No constant that happens to equal a code or data address of this program:
  pointer-to-integer casts, integer-to-pointer casts of program addresses,
  address-named alias declarations and table entries typed as plain integers
  all count as hidden fixed pointers.
- Overlays link against the built main executable's symbols, not against
  addresses copied from a retail disassembly.
- Data produced outside the build (disc assets) that embeds program addresses
  is a documented ABI. Each such place is listed with its owner, and the
  addresses it needs stay pinned on purpose.

### Measuring it

`tools/scripts/shift_audit.py` (read-only, needs `make build` and, for the
overlay table, `make overlay-build-all`) prints the inventory below;
`--json` gives the same data and `--check BASELINE` fails when a count grows.
`tools/scripts/shift_test.py` is the acceptance test. It relinks main from
the built objects into a temporary directory, with `--emit-relocs`, once as
is and once with `--pad` bytes (default 0x10) inserted at the start of
`.main`, and compares every word of the two images. A relocated word that
moved by the pad is an explained address field. A relocated word that did not
move refers to an absolute linker pin, a gp-relative offset that changed
refers to a pinned small-data symbol, and an unrelocated word shaped like a
program address (data word, `lui` upper half, `j`/`jal` target) is a literal
address. The repository is never written; the base relink must equal
`build/USA/main.exe`.

### Inventory (2026-10-05, after removing the shadowing pins)

Linked absolute assignments for main, by what the address is:

| category | manual | generated aliases | remedy |
| --- | ---: | ---: | --- |
| unreferenced by any main object | 1546 | 0 | catalogue only; move to a symbol file or drop |
| bss inside a zero-filled asm blob (295 of them already labelled there) | 298 | 1 | own the variable in a C unit's `.bss`/`.sbss` |
| tentative COMMON pinned (no BSS placement yet) | 111 | 0 | place BSS/SBSS in the manifest |
| initialised data inside an asm blob, labelled there | 108 | 1 | name the blob label, or move the data into C |
| function address that a linked object already names | 48 | 9 | reference the defining name |
| data inside a C unit's section | 15 | 26 | point at the owning symbol plus offset, or split the definition |
| hardware (I/O registers) | 10 | 0 | unavoidable |
| kernel/BIOS RAM (0x80000000, 0xA000xxxx, 0xDFFC) | 4 | 1 | unavoidable (BIOS contract) |
| symbolic aliases (`a = b;`) | 5 | 0 | not fixed; rename callers eventually |

`undefined_syms_auto.main.txt` (517) and `undefined_funcs_auto.main.txt`
(empty) are splat catalogues and are not linked.

Fixed addresses in C (comments stripped): 446 alias declarations
(`asm("name")`) whose target is an absolute pin (350 in `src/main`, 49 in
`include`, 47 in overlays), 33 integer-to-pointer casts of literal program
addresses (18 main, 15 overlays) and 5 other literal program addresses. A
further 390 address-named aliases point at symbols an object defines and do
not pin anything. The crutch ratchet's `pointer_integer_casts` (547 main, 82
overlays) is a plain `(s32)`/`(u32)` cast regex; almost all of those are
integer casts and do not bind an address.

Built main executable: 7147 relocations resolve to 618 absolute
program-range symbols (5655 address words and 1491 gp-relative offsets in the
shift test), 21 data words and 17 `lui` halves hold a program address with no
relocation at all. The literals are LIBCD BIOS state pointers
(`g_CdBiosHardwareState`), a `0x800E0xxx` function table in
`dtail_gp_pre_s016`, two string-directory buffer pointers, the
`Task_InitGpuHwRegs`/`Task_GpuFlushPrimQueue` register-block pointers at
0x80070E0x, and `0x800Axxxx` upper halves in a few CD, memory card, menu
and GPU units. A few of the 21 data words (`0x80017FFF`) are probably two
halfwords rather than an address.

Overlays never see the built main executable. Each overlay links three
generated scripts (`undefined_funcs_auto`, `undefined_syms_auto` from splat,
`undefined_extra` from `overlay_extra_undefineds.py` using
`configs/USA/sym.main.txt`), all holding retail addresses. Across the 190
overlays 8073 referenced assignments point into main (399 distinct
addresses): 6086 already match a symbol of the same name and address in the
built main ELF, 1672 match an address main names differently and 314 have
no main symbol. Overlays also pin 7539 references to their own definitions
(the same shadowing as main's, through address-named `func_`/`D_` labels) and
988 own-window addresses no object defines; 518 point into overlay windows
used as runtime buffers. Since 2026-10-06 `overlay_extra_undefineds.py` reads
only the objects the overlay's linker script links and skips every name one
of them defines: `undefined_extra` dropped from 12915 entries (4622 hard
assignments) to 1369 (471). The two overlays with copied foreign-VMA code
(`OVERLAY_VMA_OVERLAP`, `boot_display` and `menu_memcard`) keep those pins
(`--pin-defined`). The splat `undefined_*_auto` scripts still pin.

Fixed layout: `.main` at 0x80010000 (the PS-X EXE load address, a loader
contract), `.field_engine` at 0x800C1CA0, `_gp = 0x8009CD70`, and the
header literals in `asm/USA/main/header.s` (initial PC 0x80072534, text size
0x1EE000, stack 0x801FFFF0). Main's BSS, heap and every overlay load window
live inside the zero-filled `dtail_*` data blobs, so the executable covers
0x80010000 to 0x801FE000 with no layout of its own for those regions.

### Shift test result

With the script unchanged a shift does not link: `.field_engine` at its fixed
VMA overlaps the grown `.main`. With `.field_engine` and `_gp` rebased in the
scratch copy, a 0x10 shift gives 20930 explained address fields, 5655 words
still pointing at pinned symbols (464 symbols; most used `g_CurrentEntity`,
`g_AkaoCmd`, `g_AkaoCurTrack`, `g_GameState`, `_spu_RXX`), 1491 gp-relative
offsets to pinned small data, 21 literal data words, 17 literal `lui`
halves, 28 hardware references and no unexplained difference. The header
fields do not follow the shift. Before the shadowing pins were removed the
same run reported 15105 pinned words and 1947 gp-relative offsets.

### Removal order

1. Done: 931 manual assignments that duplicated a definition at the same
   address in a linked object. Byte-identical; they only hid the object's
   ownership.
2. Unreferenced manual entries (1546): no object needs them; their names
   feed `gen_expected.py`'s label table, so move the useful names into
   `configs/USA/sym.main.txt` (or drop generic ones) and check the expected
   split. Mechanical, about one commit.
3. Overlays link against the built main symbols (`ld --just-symbols` on the
   main ELF, or generated assignments taken from `build/USA/main.map`), and
   `overlay_extra_undefineds.py` stops pinning names the overlay defines
   itself (done for `undefined_extra` outside the VMA-overlap overlays).
   Clears the 8073 main references and 7539 self-shadowing pins; needs a
   check for names that exist in both main and an overlay.
4. References that a linked object already names under another label (57
   function pins, about 400 data pins into asm blobs): give the blob label the
   C name through `sym.main.txt`, or change the C reference. Mechanical per
   symbol, byte-identical.
5. Alias declarations onto pins (446) and integer-to-pointer literals (33):
   replace by the owning symbol once steps 4 and 6 give it one.
6. BSS ownership: the zero blobs and the 111 COMMON pins become `.bss`/`.sbss`
   definitions in their units with a manifest placement. This is the large
   step (several hundred variables) and is what lets `_gp` and
   `.field_engine` be derived (`ADDR(.sdata) + 0x8000`, end of `.main`)
   instead of fixed.
7. Literal words and `lui` halves (38): rewrite the C as symbol references,
   relabel the asm data words.
8. Header: initial PC from the entry symbol, text size from the section end.

Unavoidable, by design: hardware registers and the scratchpad, the kernel and
BIOS addresses (LIBCARD patch, `0x80000000` vectors), the `.main` load address
and stack top, and the overlay load addresses (0x8018EFE8 for rooms and the
other windows): the disc loader and the data on the disc depend on them.
