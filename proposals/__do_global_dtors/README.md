# `__do_global_dtors` runtime reconstruction

This reconstructs the complete 104-byte SN/GCC runtime function at
`0x8007264C`. It is the destructor-side companion to the adjacent `__main`:
after checking the shared initialization flag, it walks the linker-provided
function table and calls each entry while decrementing its count.

The source is derived from the neighboring runtime body and GCC's documented
global-constructor/destructor traversal. The loop and control flow are C. The
linker count, indirect call and four-word runtime frame remain narrow inline
assembly/compiler constraints because this SN startup module deliberately
bypasses the normal MIPS calling convention.

Stock GCC 2.7.2 produces the exact **104/104 retail bytes**. Registers `sp`,
`s0`, `s1` and `t0` are pinned. Stock `-fcall-used-$16` and
`-fcall-used-$17` options prevent GCC from adding a second save area while the
source preserves those registers in the retail 4/8/12 slots. Removing the
final empty constraint drops the `s0`/`s1` reloads; removing the `t0` pin makes
GCC allocate `at` for both the flag and indirect call.

```sh
tools/scripts/cc.sh src/main/psyq/libsn/__do_global_dtors.c /tmp/dtors.o
tools/objdiff/objdiff-cli diff \
  -1 expected/build/USA/asm/USA/main/psyq/libsn/__do_global_dtors.s.o \
  -2 /tmp/dtors.o -o /tmp/dtors.json __do_global_dtors
```
