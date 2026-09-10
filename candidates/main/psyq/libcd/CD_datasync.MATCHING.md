# CD_datasync matching state

This candidate reconstructs the complete DMA wait routine, including the
shared LIBCD timeout report and polling of DMA CHCR bit `0x01000000`. Stock GCC
2.8.1 emits `0x16C` bytes against retail's `0x168`.

Reproduce the comparison by temporarily placing `CD_datasync.c` under
`src/main/psyq/libcd/` and running:

    python3 tools/scripts/probe_match.py psyq/libcd/CD_datasync
