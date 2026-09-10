# CD_sync matching state

The recovered function implements the complete Psy-Q `CD_sync` algorithm,
including Parasite Eve's callback dispatch. With stock GCC 2.8.1 and the
recorded flags it builds to `0x284` bytes; retail is `0x280` bytes.

The first structural mismatch is register allocation: retail retains the
command-name table in `$fp`, while this candidate uses `$s5`. The extra four
bytes come from a `nop` after the first indirect callback. These two effects
shift the later schedule; no algorithmic block is missing.

Reproduce the comparison by temporarily placing `CD_sync.c` under
`src/main/psyq/libcd/` and running:

    python3 tools/scripts/probe_match.py psyq/libcd/CD_sync
