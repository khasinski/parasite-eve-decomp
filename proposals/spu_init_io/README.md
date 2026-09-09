# Combined low-level SPU initialization and FIFO transfer

The candidate compiles the adjacent _spu_init and _spu_FwriteByIO C sources
under stock GCC272, covering 1088 retail bytes. It is an incomplete source
unit: further SPU routines and storage ownership remain outside this file.
Both functions remain ASM in production. There are no pins, barriers or
instruction ASM in this combined reconstruction.

| Function | Retail bytes | Combined match |
| --- | ---: | ---: |
| _spu_init | 640 | 88.8625% |
| _spu_FwriteByIO | 448 | 93.25% |

The transfer alone scores 93.88393% with GCC281 unsplit, but that compiler
reduces initialization to 82.06875%. The common configuration keeps the
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
