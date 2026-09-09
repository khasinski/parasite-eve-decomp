# LIBCD CD_initvol

This ordinary C reconstruction matches all 240 retail bytes at `0x8007BAC0`
under stock GCC 2.8.1 with unsplit addresses. It also matches 100% in the
seven-function combined candidate with expensive optimizations disabled.
There are no register pins, barriers, stack-pointer variables or fake frames.

The routine reads the current main volume pair. Only when both are zero does
it set both master-volume registers to 0x3FFF and reload the stored SPU
pointer. It always sets CD volume to 0x3FFF/0x3FFF and SPU control to 0xC001,
then applies a local CdlATV with channels 0x80, 0, 0x80, 0. The small inline
volume helper reproduces the local packet and byte-port operations naturally.
The assignments' order reproduces the original stack stores; GCC 2.8.1 also
places stack restoration in the original return delay slot.

SpuRegs now names the existing two halfwords at offsets 0x1B8 and 0x1BA as
current_master_volume_left/right, reducing the old padding by four bytes
without changing the structure's size or subsequent offsets. These correspond
to the Current Main Volume registers documented by
[PSX-SPX](https://psx-spx.consoledev.net/soundprocessingunitspu/#1f801db8h-current-main-volume-leftright).
The u16 fields expose raw register bits, consistent with the other volume
fields; signed audio interpretation is separate from this zero test.

```sh
tools/scripts/cc.sh proposals/CD_initvol/candidate.c /tmp/CD_initvol.o
tools/objdiff/objdiff-cli diff \
  -1 expected/build/USA/src/main/psyq/libcd/bios.c.o \
  -2 /tmp/CD_initvol.o -o /tmp/CD_initvol.json
python proposals/CD_initvol/verify_behavior.py /tmp/CD_initvol.o
```

The verifier requires Unicorn and pyelftools. Its 100 cases compare returns,
restored stack, every SPU/CD register access, and complete SPU register images,
including zero/nonzero/high-bit volume values and redirecting the stored SPU
pointer after the optional master-volume writes. Replacing the both-zero
condition with an either-zero condition is rejected. This is a finite MMIO
model, not physical audio verification.

The production bios.c is deliberately not split or partially promoted. Its
neighbor CD_initintr currently matches under GCC 2.7.2 with scheduling disabled;
the clean volume routine does not match under those shared flags. Conversely,
compiling the existing or clean CD_initintr with the volume routine's GCC 2.8.1
configuration loses its match. The combined reconstruction therefore adds this
clean CD_initvol, while production retains its existing matching bios.c until
both functions can be represented correctly under one configuration.
