# CD command retry reconstruction

Candidate reconstructed from retail 0x8007A4D0..0x8007A60C.
The wrapper retries a CD command four times (counter 3 down through 0).
It saves the completion callback and temporarily clears it; conditionally
issues command 1 when status bit 0x10 is set, then command 2 when the
parameter exists and the indexed command table requests location setup.
Failure of command 2 retries immediately. Before issuing the actual command,
it restores the callback. Final failure also restores it. Return is 1 for
success (CD_cw returns zero), 0 after exhaustion.

Stock GCC281 / -mno-split-addresses compiles candidate.c to 0x134 bytes,
versus retail 0x13C. No pins, barriers, or instruction ASM. Not an exact match;
retain production ASM. Callback signature remains opaque because this wrapper
only saves/restores the pointer and never invokes it.

## Register-lifetime candidate

`natural_registers.c` removes the earlier local register pins and uses three
empty read/write barriers: initial return status, final failure status, and
the retry sentinel. Stock GCC281 now emits the retail 0x13C-byte size and
saves/restores all nine saved registers, including FP, with the correct
0x38-byte frame. Register assignment inside the body remains different, so
this is not an exact match. Explicit `-fcall-saved` experiments on the pinned
variant did not restore the missing FP save; natural allocation did.

## Constrained comparison variant

`constrained_registers.c` retains natural allocation of the retry sentinel
(FP), with nine other register pins and the three lifetime barriers. A stock
GCC281 flag sweep reduced its relocated byte differences to 44/316 by disabling
the first instruction-scheduling pass. The same constrained source with that
pass enabled differed in 78 bytes. Moving the return-status barrier alone
had not improved the previous 76-byte-difference variant.

This experimental file does not supersede the simpler candidate and is not
a match. Pins and barriers must be minimized after obtaining exact code.

## Constraint removal progress

Sequential one-at-a-time removal of constraints reduced the saved constrained
variant from 44 to **4 differing bytes out of 316 after relocation**. Removing
the table barrier, slot pin, initial status barrier, sentinel barrier, and t0
pin improved the output. The current file retains only the final status
barrier and eight register pins. Several remaining pins can individually be
removed without changing this score; their combined removal remains to test.
The function is still not byte-identical and remains ASM in the build.
