# CD_init matching state

This candidate reconstructs the complete Psy-Q `CD_init` sequence and checks
both `CdlReset` and `CdlDemute` before accepting `CD_sync == CdlComplete`.
Stock GCC 2.8.1 emits `0x1E4` bytes against retail's `0x1E0`.

An earlier `0x1E0` candidate was rejected because it ignored the return value
of `CdlReset`; equal size was therefore not a valid source match. The current
candidate retains every retail error path.

Reproduce the comparison by temporarily placing `CD_init.c` under
`src/main/psyq/libcd/` and running:

    python3 tools/scripts/probe_match.py psyq/libcd/CD_init
