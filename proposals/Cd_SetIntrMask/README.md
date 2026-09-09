# CD interrupt dispatch candidate

Reconstructed directly from retail assembly at 0x8007C13C, length 0xD8.
Despite its historical name, this function drains getintr(), dispatches ready
(bit 4) and complete (bit 2) callbacks, then restores the saved CD register bank.

candidate.c is semantic C without register pins or instruction assembly.
With native GCC 2.8.1 and -mno-split-addresses its .text is 0xE0 bytes.
The remaining eight bytes are NOP delay slots at the indirect callback calls:
the target schedules the result-buffer address addiu in each jalr delay slot.
The candidate currently materializes those addresses before each call.
This is not an exact match and must not replace the ASM manifest entry yet.
