# LIBCD CD_init

`src/main/psyq/libcd/CD_init.c` matches all 480 retail bytes at 0x8007BBFC.
Native unmodified GCC 2.7.2, `-fno-expensive-optimizations`, and GNU assembly
produce the exact function after resolving all relocations at retail addresses.
The complete executable also passes its original SHA-1 check.

Five register pins and six empty compiler barriers are recorded in the crutch
debt baseline. The pins retain independent zero arguments, prepare a0/a1 before
the event-state address, and preserve the final comparison in a0/v1. A tied
pointer barrier keeps the shared event-state base; empty control-flow barriers
preserve the distinct failure path and success jump. There is no instruction
inline assembly, ABI-changing flag, compiler patch, or assembler patch.

GCC 2.7.2 produces the correct stack restoration before `jr ra`. The previous
GCC 2.8.1 candidate placed it in the return delay slot. Source constraints now
also match the 2.7.2 body, so production uses C.

`verify_behavior.py` passes 240 cases covering return values, cleared state,
interrupt registration, MMIO order, command order, and command failure paths.

```sh
tools/scripts/cc.sh src/main/psyq/libcd/CD_init.c /tmp/CD_init.o
.venv/bin/python proposals/CD_init/verify_behavior.py /tmp/CD_init.o
make check
make debt-check
```
