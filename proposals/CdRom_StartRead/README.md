# Start CD read

Exact C reconstruction of the 304-byte function at 0x80080E34, using
`CdlLOC` and the recovered `CdReadProgressState`. Its empty constraints are
tracked as debt. The project labels this symbol main-game; upstream SDK
attribution is not established here.

A progress flag equal to one returns zero immediately; otherwise a nonzero
DsRead_IsBusy result also rejects the request. Before issuing a command the
function records sector size 512, destination and sector count. It copies the
supplied four-byte position, or gets the current position when none is supplied.
The command receives the low byte of mode|0x20, that local copy, command six,
CdRom_SetMode2Callback and stack argument -1. Failure returns zero with the
already-written request state intact. Success records VSync(-1), conditionally
installs the data callback and saves its previous value, sets inProgress to
one and returns the original command result (not a normalized boolean).

Production now matches all **304/304 linked bytes** using native GCC 2.7.2
and unmodified GNU as. Two register pins and four empty constraints are
recorded in the debt baseline; there is no instruction asm. The state anchors
preserve the recovered layout. Both position branches issue the same command;
GCC merges their tails with the retail argument scheduling. The entry
constraint preserves the saved-register order and incoming mode lifetime.

```sh
tools/scripts/cc.sh proposals/CdRom_StartRead/candidate.c /tmp/cd-start.o
python proposals/CdRom_StartRead/verify_behavior.py /tmp/cd-start.o
```

The SHA-1-checked retail oracle passes 432 cases spanning three progress
states, busy/idle responses, supplied/fallback unaligned positions, three
modes paired with sector counts -1/0/3, command results zero/one/-1, both
callback flags and a flag change during VSync. It compares state snapshots at
API calls, copied position bytes, all command arguments including the fifth
stack argument, callback identity/save, final state, return and SP. APIs are
modeled with NOP entry stubs, not executed bodies. Mutants broadening the guard
to any nonzero progress value, setting mode bit 0x10 instead of 0x20, or
normalizing the return to one fail assertions.
