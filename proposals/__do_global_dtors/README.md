# `__do_global_dtors` runtime reconstruction

This reconstructs the complete 104-byte SN/GCC runtime function at
`0x8007264C`. It is the destructor-side companion to the adjacent `__main`:
after checking the shared initialization flag, it walks the linker-provided
function table and calls each entry while decrementing its count.

The source is derived from the neighboring runtime body and GCC's documented
global-constructor/destructor traversal. The indirect call and linker count
materialization remain inline assembly because this runtime stub deliberately
does not use the normal MIPS outgoing-call frame. The loop and control flow are
expressed in C.

Stock GCC 2.7.2 produces the exact retail size and scores **99.38461%**.
Every body instruction agrees. The only differences are the prologue and
epilogue save offsets: the candidate saves `s0`, `s1`, and `ra` at 0/4/8,
while retail leaves the first word unused and saves them at 4/8/12. Adding a
normal C indirect call creates a 32-byte frame and a 108-byte function, so it
does not represent this runtime ABI. Production remains assembly until the
four-word frame convention is reproduced with the stock pipeline.

```sh
tools/scripts/cc.sh proposals/__do_global_dtors/candidate.c /tmp/dtors.o
tools/objdiff/objdiff-cli diff \
  -1 expected/build/USA/asm/USA/main/psyq/libsn/__do_global_dtors.s.o \
  -2 /tmp/dtors.o -o /tmp/dtors.json __do_global_dtors
```
