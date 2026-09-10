# CD_cw matching state

This candidate reconstructs the complete command writer from Psy-Q `BIOS_1`:
validation, position/mode state updates, parameter FIFO writes, command issue,
blocking interrupt dispatch, result copying, and disk-error return value.

Among the locally measured semantic variants, this is the closest: stock GCC
2.8.1 emits `0x410` bytes against the retail `0x40C`, with 208 differing words.
The remaining mismatch is code generation and scheduling; the candidate has no
known missing control-flow path.

Reproduce the comparison by temporarily placing `CD_cw.c` under
`src/main/psyq/libcd/` and running:

    python3 tools/scripts/probe_match.py psyq/libcd/CD_cw
