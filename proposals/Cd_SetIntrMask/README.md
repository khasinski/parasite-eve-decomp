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

## Follow-up experiments

Removing `-mno-split-addresses` fills the callback delay slots, but hoists
addresses into additional saved registers and increases the stack frame from
0x28 to 0x30. The resulting text remains 0xE0, so this is not a match.
Trials disabling strength reduction, either instruction scheduler, or expensive
optimizations also retained 0xE0 text in that configuration.

With the saved candidate, callback declarations using unspecified arguments,
`unsigned char` status, or `void *` buffers each retained 0xE0 text.
MASPSX ASPSX profiles 1.07, 2.08, 2.21, 2.34, 2.79, and 2.86 also retained
0xE0 text. These size checks rule out an exact match for those trials; they
are not relocated instruction comparisons or proof of SDK provenance.
