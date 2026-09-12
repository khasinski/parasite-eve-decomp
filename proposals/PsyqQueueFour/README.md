# Four Psy-Q byte matches

| SDK function | Preserved link symbol | Library / object | Retail address | Text |
| --- | --- | --- | --- | ---: |
| DsCommand | Render_AllocParticleNode | LIBDS / DSSYS_2.OBJ | 0x8007EE84 | 580 bytes |
| DsPacket | Render_BuildParticleFrame | LIBDS / DSSYS_2.OBJ | 0x8007F0C8 | 848 bytes |
| DsSync | Render_FindParticleEffect | LIBDS / DSSYS_2.OBJ | 0x8007F418 | 496 bytes |
| _copy_memcard_patch | func_8007E4E0 | LIBCARD / PATCH.OBJ | 0x8007E4E0 | 52 bytes |

All 1,976 text bytes match retail after linking, including branch destinations
and relocated references. DsPacket's complete 100-byte jump table at
0x80011C9C also matches. The build uses unmodified native GCC 2.7.2 and the
existing GNU assembler route. No compiler, assembler, ABI flags or instruction
postprocessing changed. The added C contains no instruction-bearing inline asm.

The misleading Render_* names were already present. SDK provenance was checked
against the Psy-Q 4.3 signature JSON files in
`psx-ruby/.tools/psyq-sigs/430/{LIBDS,LIBCARD}.LIB.json`.
`provenance.json` records object names, symbol offsets, signature-file SHA-256,
and the number of compared and relocation-wildcard bytes. Every non-wildcard
byte matches over each complete function range. The retail-free regression test
then verifies the **entire linked byte range without masking any bytes**.
These four C units are classified as Psy-Q in the progress report.

The existing one-argument symbol `DsSync` at 0x8007FBF0 is a separate internal
queue-state helper. The newly recovered SDK DsSync is the two-argument routine
at 0x8007F418. Link symbols remain unchanged to avoid unrelated caller changes.
`CopyMemcardPatch` splits the existing assembly tail at its actual function
boundaries; the surrounding patch/BIOS routines remain assembly.

## Types and behavior

`pe1/psyq_ds_queue.h` shares the existing 24-byte pending queue entry and adds
DSSYS_2's 16-byte stack packet and four-word result union. Layout assertions cover
payload, parameter and callback offsets. Callback and count words are forwarded
unchanged. The callback ABI is not inferred from forwarding alone.

DsCommand optionally queues a Setloc command before the requested command.
Each enqueue advances the nonzero serial; failure of the second enqueue leaves
the first queued command intact. DsPacket creates the four-command sequence
Pause, Setmode, Setloc, requested command, using a single serial. It accepts
command bytes 3, 6, 21, 22 and 27; allocation failure can leave a partial packet.
The mode packet initializes one payload byte; its remaining three stack bytes
are copied as in retail, not synthesized as zero.

DsSync scans the eight-entry completion history, searches newest first for the
requested ID, publishes all four words and copies the eight result bytes. ID
zero examines only the most recent entry (an empty entry is unavailable).
Pending and unavailable results return
0 and 6 respectively. The copy helper moves the complete 112-byte patch image
from 0x8007E344 to 0xDF80, stopping at 0x8007E3B4.

## Remaining debt

Pins and empty constraints were removed individually whenever the complete
linked bytes still matched, then checked again after introducing shared types.
Remaining constraints preserve register allocation, memory scheduling, known
zero assignments and the original duplicated packet enqueue paths. They are
included in `tools/crutch_debt_baseline.json` and `make debt`:

- 12 register pins and 31 empty barriers in the four new functions.
- 17 explicit gotos, chiefly the history search and packet failure paths.
- DsSync retains word-offset views of the history at D_800A3610/14/18/1C;
  DsPacket retains a callback-word cursor through its stack records.

These are byte-matched C functions with declared debt, not claimed to be clean
C translation units. No new instruction asm, symbol aliases or toolchain patches
are needed. PutDispEnv was investigated separately but is not part of this change
and receives no new progress credit.

## Validation

- `make -j8 check`: entire main.exe matches retail SHA-1
  `452fb033f2eaa4b18aa20a5bca60b8125af3a37b`.
- `make ci`: 317 tests, including `test_psyq_queue_four.py`, which links all four
  sources at their retail addresses and checks full text and jump-table hashes.
- `.venv/bin/python proposals/PsyqQueueFour/verify_behavior.py`: 2,048 cases,
  each executed with retail and rebuilt code and compared with independent
  state models (512 per function). Covers queue limits, optional parameters,
  serial wraparound, accepted/rejected commands, partial allocation failure,
  dispatch conditions, history wraparound/duplicates/missing IDs, signed IDs,
  full result copies, randomized stack padding, patch data and guard bytes.
  Every run also checks stack and callee-saved register preservation. Hardware
  and queue allocation helpers are explicitly modeled; this is not a hardware
  integration test.
- `make report`: audit credits 2,397,800 semantic-C bytes / 10,509 functions,
  an increase of 1,976 bytes / four functions.
