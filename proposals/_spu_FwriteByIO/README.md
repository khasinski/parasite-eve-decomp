# SPU FIFO transfer

`candidate.c` reconstructs the complete 448-byte retail function at 0x8007D454
using SpuRegs, halfword source accesses and ordinary C loops. It has no pins,
barriers or instruction ASM. Stock GCC281 unsplit scores 92.45536%, unchanged
by disabling expensive optimizations. GCC272 scores 91.82143%; disabling its
second scheduler scores 80.47321%. The production segment remains ASM at
psyq/misc32. The label func_8007D5AC in the old dump is inside the final wait,
not a separate callable function.

The function saves the low eleven SPU status bits, writes the transfer address
and waits once. Each chunk is at most 64 bytes. Its FIFO loop copies halfwords,
then selects manual-write mode, waits and polls status bit 0x400. A timeout
prints a diagnostic and continues to the two settling waits and the next
chunk. After all chunks, it clears transfer-mode bits and polls until the
low eleven status bits equal the saved value; this wait also times out with
a diagnostic. Size zero still sets the transfer address and restores mode.

For an odd size, the last FIFO access still reads a whole halfword. The tests
provide an extra accessible byte and preserve this observed retail behavior;
this is not a claim that arbitrary odd-sized caller buffers are safe.

```sh
tools/scripts/cc.sh proposals/_spu_FwriteByIO/candidate.c /tmp/spu-io.o
tools/objdiff/objdiff-cli diff \
  -1 expected/build/USA/asm/USA/main/psyq/misc32.s.o \
  -2 /tmp/spu-io.o -o /tmp/spu-io.json
python proposals/_spu_FwriteByIO/verify_behavior.py /tmp/spu-io.o
```

The 128-case Unicorn comparison covers lengths 0/1/2/63/64/65/128/129,
status transitions after 0/1/3840/3841 busy reads, zero/A5 register fills and
optional register-pointer replacement by the first modeled wait. It compares
all register accesses, FIFO words, source addresses, diagnostics, final SPU
register contents and stack. It independently checks chunk and wait counts,
both timeout thresholds and restoration of the masked initial status.
The tested size domain is finite, with aligned and sufficiently padded inputs.

Wait and printf are modeled through two-word NOP stubs and second-word hooks,
following the initialization verifier's handling of call delay slots. This is
not physical SPU timing verification or a byte-exact match. Negative controls
for the timeout comparison, 32-byte chunks, a nonadvancing source pointer and
status bit 0x200 instead of 0x400 are all rejected.
