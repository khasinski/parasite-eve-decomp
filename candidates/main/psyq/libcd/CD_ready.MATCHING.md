# CD_ready matching state

This is the complete Psy-Q `CD_ready` algorithm adapted to Parasite Eve's
three-byte interrupt state and callback dispatch. Stock GCC 2.8.1 with
`-mno-split-addresses` emits `0x2D4` bytes; retail is `0x2C8` bytes.

The function has both result paths: the data-end event copies the third eight
byte result buffer, and the ready event copies the second. The remaining work
is register allocation and instruction scheduling, not missing behavior.

Reproduce the comparison by temporarily placing `CD_ready.c` under
`src/main/psyq/libcd/` and running:

    python3 tools/scripts/probe_match.py psyq/libcd/CD_ready
