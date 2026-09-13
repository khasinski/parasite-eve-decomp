# Four Psy-Q functions: PADMAIN and FIRST

| Function / existing symbol | SDK object | Retail address | Bytes |
| --- | --- | --- | ---: |
| _padStartCom / MemCard_StartCounterIrq | LIBPAD/PADMAIN.OBJ | 0x80082CF0 | 204 |
| Private state machine / MemCard_RunCommandStep | LIBPAD/PADMAIN.OBJ, text_770 | 0x80083014 | 200 |
| _padSioRW / MemCard_ReadByte | LIBPAD/PADMAIN.OBJ | 0x800830DC | 472 |
| firstfile wrapper / Scene_CreateEntityNode | LIBAPI/FIRST.OBJ | 0x800727B4 | 412 |

All 1,288 linked text bytes match retail without masking relocations. The two
adjacent PADMAIN functions share `psyq/libpad/padmain_sio.c`; their combined
672-byte range and individual function hashes are tested. Legacy link symbols
remain unchanged. The existing BIOS trampoline named `firstfile` is distinct
from the SDK wrapper recovered here.

`provenance.json` records full-range comparisons against Psy-Q 4.3 signature
JSON from `psx-ruby/.tools/psyq-sigs/430/`, including source hashes, SDK offsets,
and relocation-wildcard counts. Every known byte in every function matches.
The private PADMAIN function has only the generated SDK signature label
`text_770`; no original private C name is claimed. It ends immediately before
SDK `_padSioRW` at offset 0x838. SDK signatures identify provenance; the unit
test independently checks every linked byte, including all relocations.

## Source and types

`psyq_pad_main.h` shares PADMAIN state and callback declarations. The existing
CardObj and SIO/interrupt-register layouts are reused. CardObj's byte at 0x44
is now `response_index`, alongside `payload_index` at 0x45, with layout checks.
The two port objects retain their 0xF0-byte stride. Existing MemCard/Spu names
in this region do not imply that these functions came from LIBCARD/LIBSPU.

The start routine registers the interrupt record, configures I_STAT/I_MASK,
initializes both ports and clears the two RAM counters before enabling polling.
The state machine advances its callback index, handles errors and readiness
waits, and caps the next index by decrementing values of five or more.
The SIO routine distinguishes negative setup commands from ordinary exchanges,
polls hardware, handles timeout -20, and updates the receive buffer and indices.
MMIO byte/halfword/word accesses retain their original widths and ordering.

The FIRST wrapper extracts a signed-byte device-name prefix, searches the
0x50-byte BIOS device records, saves the original firstfile handler, searches
again to install the existing hook, and invokes the BIOS trampoline. It uses
the existing BiosDeviceEntry structure. Both table scans and their reloads
remain visible in the source.

## Validation and debt

- Native, unmodified GCC 2.7.2 and the existing GNU assembler route.
- `make -j8 check`: full main.exe equals retail, SHA-1
  `452fb033f2eaa4b18aa20a5bca60b8125af3a37b`.
- `make ci`: 318 tests, including retail-free full-byte hashes for all four
  functions and the combined PADMAIN source unit.
- Layout checks for the recovered receive and payload cursor fields.
- No toolchain edits or instruction-bearing inline ASM added.
- Remaining debt: six pins, eight empty barriers, nine gotos; recorded in
  `tools/crutch_debt_baseline.json`. The FIRST wrapper has no pins and one
  memory/register barrier; PadStartCom has no pins and two barriers.

The pre-existing `asm_constrained_units` count stays at 28 and the legacy
`asm_bodies` counter stays at 71. These are not new-function counts. The latter
uses a regex which can miscount symbol aliases as instruction blocks, so it
must not be interpreted as a verified number of CPU-ASM blocks. All three new
source units classify as semantic C. The whole-EXE match and regression tests
verify the emitted program; hardware behavior is not claimed to have been
independently exercised on a console in this change.
