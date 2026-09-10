# Psy-Q LIBCD C_011 reconstruction

This directory contains a source reconstruction of the complete exported
`StCdInterrupt` routine from Psy-Q 4.6 `LIBCD.LIB` member `C_011.OBJ`.
The source shape was adapted from the MIT-licensed SOTN decompilation and all
SDK-local globals were mapped independently to this executable by following
the retail loads, stores, and the neighboring stream API functions.

Stock GCC 2.8.1 with `-mno-split-addresses` emits **2332 bytes** for
`StCdInterrupt`, exactly the retail function size (`0x91c`), and objdiff reports
**90.69297%**. The candidate also emits the following 44-byte `mem2mem` helper
exactly. Production remains on the retail assembly until the exported routine
matches exactly; this candidate is now the source-level basis for completing
the entire translation unit instead of treating its routines as unrelated
files.

The recovered `StHEADER` layout is 32 bytes. Its first five halfwords are the
stream id, type, sector index, sector count, and frame number; `CdlLOC` occupies
the final four bytes. This agrees with the ring stride used by `StGetNext`,
`StFreeRing`, and `init_ring_status`.

```sh
tools/scripts/cc.sh proposals/StCdInterrupt/candidate.c /tmp/StCdInterrupt.o
tools/objdiff/objdiff-cli diff \
  -1 build/USA/asm/USA/main/psyq/libcd/StCdInterrupt.s.o \
  -2 /tmp/StCdInterrupt.o -o /tmp/StCdInterrupt.json StCdInterrupt
```
