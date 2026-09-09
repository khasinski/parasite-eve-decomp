# Combined SPU core reconstruction

`candidate.c` compiles fourteen contiguous retail functions (0x8007D1D4 through
0x8007DD08, 2868 bytes) with stock GCC272 and upstream MASPSX --expand-div.
Sources are shared with the existing candidates and production functions.
This is a source reconstruction of the code range, not a completed original
TU: trailing padding and data ownership remain unresolved. Production manifests
and compiler boundaries are unchanged.

| Function | Retail bytes | Combined match |
| --- | ---: | ---: |
| _spu_init | 640 | 95.8125% |
| _spu_FwriteByIO | 448 | 93.25% |
| _spu_FiDMA | 188 | 100% |
| _spu_Fr_ | 168 | 96.190475% |
| _spu_t | 640 | 96.75% |
| _spu_Fw | 132 | 95.15151% |
| _spu_Fr | 100 | 93.6% |
| _spu_FsetRXX | 68 | 100% |
| _spu_FsetRXXa | 164 | 97.560974% |
| _spu_FgetRXXa | 60 | 100% |
| _spu_FsetPCR | 88 | 100% |
| _spu_FsetDelayW | 40 | 100% |
| _spu_FsetDelayR | 40 | 100% |
| _spu_Fw1ts | 92 | 93.04348% |

There is no instruction ASM in the collected source. The existing five
register pins and one tied barrier in _spu_FsetRXXa remain, as does the one
v1-clobber barrier in _spu_FiDMA. The latter scores 97.87234% without its
barrier; signed/postincrement/for-loop alternatives scored 96.59574%,
92.87234% and 97.85107%, so none is retained. The PSX varargs implementation
in _spu_t is inherited from the production source.

The wait now uses real stack locals and unsigned multiplication. It matches
all 92 bytes standalone with GCC281 but needs further scheduling work under
the common GCC272 configuration. It replaces the synthetic SP model only in
this reconstruction. Other pre-existing production C functions likewise keep
their current compiler choices until the full group can match together.

```sh
tools/scripts/cc.sh proposals/spu_core/candidate.c /tmp/spu-core.o
python proposals/_spu_init/verify_behavior.py /tmp/spu-core.o --real-io
python proposals/_spu_FwriteByIO/verify_behavior.py /tmp/spu-core.o
python proposals/_spu_Fw1ts/verify_behavior.py /tmp/spu-core.o
```

The 192 initialization/real-transfer cases, 128 FIFO cases and two complete
wait-loop traces pass. Wait and diagnostics remain modeled in the first two
suites; wait itself is executed in the third. This is not an integrated DMA
interrupt or whole-library behavior proof. API hooks resolve candidate ELF
entries so internal calls do not silently target their old retail addresses.

Combining the sources exposed an incompatible unsigned-char base pointer in
_spu_FsetRXXa. It now uses SpuRegs* in the candidate and production source,
while retaining the existing integer address calculation. This changes no
retail bytes: main SHA, all 191 overlay SHAs and source/organization/debt gates
pass. No production function has been promoted based on the combined scores.

DMA MADR/BCR/CHCR pointers and the direction word now have shared declarations
in psyq_spu_internal.h, alongside the wait, FIFO-write and delay-setting
prototypes. Twelve duplicate production declarations and three candidate
prototypes were removed. The existing shared shift-count declaration replaces
two local copies. All fourteen combined scores remain unchanged; main retail
SHA, all 191 overlay SHAs, 290 repository tests and source/organization/debt
gates pass. The production extern count falls from 3610 to 3603.

The transfer-address qualifier conflict remains unresolved: spu_transfer uses
ordinary u16 storage while Spu_WriteRegChecked uses volatile u16. The latter's
retail code stores then reloads the halfword; the former's polling code does
not reload it on every iteration. Making the core declaration volatile gives
_spu_t 91.4625% while its three siblings remain 100%. Removing volatile from
the checked wrapper gives 74.78261% under GCC272, or 87.82609% under GCC281
unsplit. Neither is accepted into production.

An ordinary declaration with a volatile read view gives 95% in the wrapper,
but introduces an extra address ADDIU before LHU. Making both accesses use
volatile views gives 90.434784%. No-force-addr and no-force-mem retain 95%;
no-expensive-optimizations gives 89.347824%. A two-member union with ordinary
and volatile halfword views preserves all four core functions, but the wrapper
scores 90.434784% whether its store uses the ordinary or observed member.
These access-view experiments are not retained or asserted as an original
structure. Resolving this boundary requires matching both users, not simply
choosing one declaration globally.
