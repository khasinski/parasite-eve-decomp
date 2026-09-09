# _spu_Fw1ts: local delay workspace

`candidate.c` reconstructs the 92-byte delay function using an ordinary local
`SpuWaitWork`, without pins, barriers or instruction assembly. Stock GCC 2.8.1
with `-mno-split-addresses` gives 100% objdiff against the SHA-verified production
object. All 92 bytes agree after accounting for the function's section offset
in its single local `R_MIPS_26` jump (instruction +12 targets function +64).
The volatile arithmetic and polling accesses are retained.

Production currently belongs to `psyq/libspu/spu_register_write.c`, which uses
GCC 2.7.2. Replacing its stack-register variable with this local workspace under
2.7.2 moves stack restoration before the return and inserts a return-slot nop
(93.04348%). Switching the whole TU to stock 2.8.1 fixes this function, but
changes `_spu_FsetRXX` (87.05882%), `_spu_FsetRXXa` (89.756096%) and
`_spu_FgetRXXa` (78.333336%). The PCR and two delay-register helpers remain 100%.

This is therefore a candidate, not a production promotion. Do not split the
existing TU just to select a different compiler for one function. Continue
source reconstruction for the other three functions under the same profile.
The production TU also retains instruction assembly in `_spu_FsetRXXa`.

```sh
tools/scripts/cc.sh proposals/SpuWait/candidate.c /tmp/spu-wait.o
tools/objdiff/objdiff-cli diff \
  -1 build/USA/src/main/psyq/libspu/spu_register_write.c.o \
  -2 /tmp/spu-wait.o -o /tmp/spu-wait.json _spu_Fw1ts
```

Related trials: deleting `_spu_FiDMA`'s counter clobber gives 97.87234% and
inserts a compensating decrement on loop exit. Tested loop shapes and stock
scheduler/CSE flags did not remove it. `_SpuInit` can use a typed backward
pointer at 100%, but that source would form a pointer before the array on the
last iteration; bounded-pointer and indexed-loop variants remain nonmatching
and were not accepted.
