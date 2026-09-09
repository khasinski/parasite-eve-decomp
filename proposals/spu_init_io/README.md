# Combined low-level SPU initialization and FIFO transfer

The candidate compiles the adjacent _spu_init and _spu_FwriteByIO C sources
under stock GCC272, covering 1088 retail bytes. It is an incomplete source
unit: further SPU routines and storage ownership remain outside this file.
Both functions remain ASM in production. There are no pins, barriers or
instruction ASM in this combined reconstruction.

| Function | Retail bytes | Combined match |
| --- | ---: | ---: |
| _spu_init | 640 | 88.8625% |
| _spu_FwriteByIO | 448 | 91.82143% |

The transfer alone scores 92.45536% with GCC281 unsplit, but that compiler
reduces initialization to 82.06875%. The common configuration keeps the
better initialization result rather than claiming both individual maxima.

```sh
tools/scripts/cc.sh proposals/spu_init_io/candidate.c /tmp/spu-init-io.o
python proposals/_spu_init/verify_behavior.py /tmp/spu-init-io.o
python proposals/_spu_FwriteByIO/verify_behavior.py /tmp/spu-init-io.o
```

The 48 initialization and 128 transfer comparisons pass on the common object.
Initialization still models the transfer call: its hook now resolves the
candidate's actual ELF entry. This confirms both functions under one compiler,
not execution of their complete interaction. The wait and diagnostic calls
are modeled in both suites. Shared SpuRegs/SpuVoiceRegs declarations make the
register accesses consistent across these two bodies.
