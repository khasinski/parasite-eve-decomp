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

## Full TU reconstruction under GCC 2.8.1

`register_write_gcc281.c` now gives 100% for six of seven functions in the
shared unit: `_spu_FsetRXX`, `_spu_FgetRXXa`, `_spu_FsetPCR`, both delay-register
setters and `_spu_Fw1ts`. It contains no CPU instruction assembly.

An empty volatile exit barrier preserves the shared return in `_spu_FsetRXX`.
The getter performs one typed volatile read, then selects shifted or raw
value and reaches a shared return. It needs an `a0` value pin and an empty
exit barrier. Memory clobbers are unnecessary for these exit barriers.
The opposite condition ordering changes block layout and does not match.

`_spu_FsetRXXa` is still incomplete: 84.02439% when its hand-expanded division
is replaced by `value % unit`. Keeping the old instruction assembly during
diagnosis scores 93.78049%, but that is not the saved C reconstruction.
The saved candidate also removes the now-unnecessary local pin on `unit`.
No production compiler profile or match accounting has changed.

```sh
tools/scripts/cc.sh proposals/SpuWait/register_write_gcc281.c /tmp/spu-registers.o
tools/objdiff/objdiff-cli diff \
  -1 build/USA/src/main/psyq/libspu/spu_register_write.c.o \
  -2 /tmp/spu-registers.o -o /tmp/spu-registers.json
```

## Remaining remainder-operation nop

The full candidate now improves `_spu_FsetRXXa` from 84.02439% to
**97.560974%**, retaining 100% for the other six functions. The stock
`MASPSX_FLAGS: --expand-div` option restores the retail divide-by-zero guard.
Separating `ret` (the intermediate shifted value) from `result` (the common
return value) restores the switch's register choices and shared exit. The
local offset pin is also removable with no change in score.

The only instruction difference is an extra nop immediately after `mfhi`,
before the branch testing its result. This shifts subsequent local jump
addresses; it is not an exact byte match. The pinned upstream MASPSX
`divu/remu` handler falls through to `_handle_nop_before_next_instruction`
after emitting the remainder expansion. Its older-version setting for
`nop_mflo_mfhi` does not disable this separate dependent-instruction path.
Profiles 2.21, 2.30 and 2.60 retain this extra nop; the older profiles also
change the wait helper. No assembler modification or CPU assembly is used.
