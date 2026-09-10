# `__main` SN/GCC runtime reconstruction

This reconstructs the complete 112-byte runtime entry at `0x800725DC`. It
guards constructor execution with `D_80094538`, sets that flag on the first
call, and traverses the linker-provided constructor table. Together with
`../__do_global_dtors/candidate.c`, it recovers the two adjacent halves of the
SN/GCC global initialization runtime.

The linker count and its non-ABI indirect call remain inline assembly because
the original stub bypasses the normal MIPS outgoing-call area. Stock GCC 2.7.2
produces the exact retail size and scores **97.28571%**. The constructor loop is
instruction-identical. Remaining differences are the same four-byte frame
layout seen in `__do_global_dtors` and the encoding of constant one: ordinary C
emits `addiu`, while the handwritten retail stub uses `ori`. Forcing `ori` with
an isolated asm expression prevents delay-slot scheduling and grows the body,
so that experiment is not retained. Production remains assembly pending a
stock-pipeline match for the shared runtime convention.

```sh
tools/scripts/cc.sh proposals/__main/candidate.c /tmp/sn-main.o
tools/objdiff/objdiff-cli diff \
  -1 expected/build/USA/asm/USA/main/psyq/libsn/__main.s.o \
  -2 /tmp/sn-main.o -o /tmp/sn-main.json __main
```
