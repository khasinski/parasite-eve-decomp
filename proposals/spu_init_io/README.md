# Combined low-level SPU initialization and FIFO transfer

The candidate compiles the adjacent _spu_init and _spu_FwriteByIO C sources
under stock GCC272, covering 1088 retail bytes. It is an incomplete source
unit: further SPU routines and storage ownership remain outside this file.
Both functions remain ASM in production. There are no pins, barriers or
instruction ASM in this combined reconstruction.

| Function | Retail bytes | Combined match |
| --- | ---: | ---: |
| _spu_init | 640 | 95.8125% |
| _spu_FwriteByIO | 448 | 93.25% |

The transfer alone scores 93.88393% with GCC281 unsplit, but GCC281 has not matched the initialization body; the previous
three-store variant scored 85.06875% with it. The common configuration keeps the
better initialization result rather than claiming both individual maxima.

```sh
tools/scripts/cc.sh proposals/spu_init_io/candidate.c /tmp/spu-init-io.o
python proposals/_spu_init/verify_behavior.py /tmp/spu-init-io.o
python proposals/_spu_init/verify_behavior.py /tmp/spu-init-io.o --real-io
python proposals/_spu_FwriteByIO/verify_behavior.py /tmp/spu-init-io.o
```

The 48 initialization and 128 transfer comparisons pass on the common object.
By default initialization models its transfer call, resolving the candidate
ELF entry. `--real-io` executes the actual _spu_FwriteByIO body on each side:
retail calls retail, and the candidate calls the compiled C function. Entry
assertions require the C transfer to reside in the candidate text region.

That mode passes 192 cases: three cold/hot values, four reset-status delays,
four independent transfer/restore delays, two initial fills, and optional
SPU pointer replacement by the first wait. It checks call arguments, the eight
payload halfwords written to the FIFO, all register traces and software state,
zero return, stack, and 13 waits on cold start versus one on hot start.
Both transfer timeout paths and reset timeout continuation are exercised.
Omitting the transfer call is rejected as a negative control. Wait and printf
remain modeled; this is finite integration evidence, not physical timing or
a byte match. Shared SpuRegs/SpuVoiceRegs declarations make the register
accesses consistent across these two bodies.

Initialization's single-store refinements recover three call delay slots and
retain the IRQ callback reset after SPU enable. The combined init score is now
92.9875%, while transfer stays at 93.25%. The shared trace now includes both
callback-word writes; reverting the ordered IRQ store is rejected. All 192
real-transfer cases, 48 modeled-transfer cases and 128 transfer cases pass.

Volatile initialization-buffer writes now retain the ascending retail order;
the shared volatile IRQ callback declaration removes the local access cast.
The initializer reaches 95.8125%. Its oracle records queue writes as well as
callback/MMIO ordering, and rejects both an ordinary buffer declaration and a
nonvolatile IRQ reset view. All 240 initialization comparisons pass.
