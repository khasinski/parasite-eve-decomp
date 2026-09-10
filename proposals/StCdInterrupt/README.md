# Psy-Q LIBCD C_011 reconstruction

This directory contains a source reconstruction of the complete exported
`StCdInterrupt` routine from Psy-Q 4.6 `LIBCD.LIB` member `C_011.OBJ`.
The source shape was adapted from the MIT-licensed SOTN decompilation and all
SDK-local globals were mapped independently to this executable by following
the retail loads, stores, and the neighboring stream API functions.

The retail `StCdInterrupt` is **2332 bytes** (`0x91c`). Stock GCC 2.8.1 with
`-mno-split-addresses` currently emits 2408 bytes for the reconstruction and
objdiff reports **90.79588%**. The candidate also emits the following 44-byte `mem2mem` helper
exactly. Production remains on the retail assembly until the exported routine
matches exactly; this candidate is now the source-level basis for completing
the entire translation unit instead of treating its routines as unrelated
files.

The shared `StHEADER` now uses the official Psy-Q `LIBCD.H` fields. It is 32
bytes, `frameCount` begins at offset 8, and `CdlLOC` occupies the final four
bytes. This agrees with the ring stride used by `StGetNext`, `StFreeRing`, and
`init_ring_status`.

The interrupt reconstruction now names the `type`, `secCount`, and `nSectors`
accesses through that shared structure. These accesses preserve every
generated instruction. The remaining halfword access at offset 8 is
intentional: retail reads only the low 16 bits of the 32-bit `frameCount`
field. Naming it as a full-width field changes the load and is incorrect for
this routine. The `id` stores also retain their halfword view because changing
their expression alters GCC's scheduling; both are documented exceptions in
an otherwise typed header view.

The payload destination is now expressed in bytes: the ring stores all
32-byte `StHEADER` records first, followed by one 0x7E0-byte payload area per
entry. The previous `StHEADER *` expression multiplied the payload index by
`0x3f` headers to obtain the same 2016-byte stride, obscuring this layout.
The explicit byte calculation produces identical text and relocations while
making the two-array organization of the streaming ring visible in C.

```sh
tools/scripts/cc.sh proposals/StCdInterrupt/candidate.c /tmp/StCdInterrupt.o
tools/objdiff/objdiff-cli diff \
  -1 build/USA/asm/USA/main/psyq/libcd/StCdInterrupt.s.o \
  -2 /tmp/StCdInterrupt.o -o /tmp/StCdInterrupt.json StCdInterrupt
```
