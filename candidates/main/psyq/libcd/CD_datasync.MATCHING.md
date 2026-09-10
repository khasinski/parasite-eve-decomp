# CD_datasync matching state

This candidate reconstructs the complete DMA wait routine, including the
shared LIBCD timeout report and polling of DMA CHCR bit `0x01000000`. For this source shape, stock GCC 2.7.2 emits `0x174` bytes against retail's `0x168` and reproduces the retail prologue, saved-register count, and code through offset `0x94`. It has 49 reloc-masked differing words after retaining the timeout limit in `$s4`; the earlier 2.8.1 candidate had 83. This is evidence for the current reconstruction profile, not proof that all of `BIOS_1.OBJ` used one compiler version.

Reproduce the comparison by temporarily placing `CD_datasync.c` under
`src/main/psyq/libcd/` and running:

    python3 tools/scripts/probe_match.py psyq/libcd/CD_datasync
