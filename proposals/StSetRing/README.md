# StSetRing and the streaming boundary

`candidate.c` reconstructs `StSetRing` using LIBCD.H's pointer and 32-bit
size parameters. It stores `StRingAddr`, stores `StRingSize`, and calls
`StClearRing`. The SDK names alias the existing retail data symbols.
No instruction ASM, pins or barriers are used.

Psy-Q 4.6 LIBCD.LIB's **CDROM.OBJ** exports StSetRing at text offset zero.
Its 48 text bytes match retail **0x8007A214..0x8007A244** modulo exactly
five relocations: two HI/LO pairs for StRingAddr and StRingSize, and one
J26 for StClearRing. All other instruction bits and the trailing zero word
match. The callable function occupies 44 bytes; the object has four trailing
zero bytes. CDROM.OBJ also defines the streaming globals as common BSS and
has 16 initialized bytes containing the four CD controller register addresses.
The candidate currently references the existing globals rather than owning
all of that object storage.

The current manifest incorrectly folds the function into
`psyq/libgte/St_DmaCompleteCallback`, whose symbol begins at 0x8007A1E0.
The preceding 0x30 bytes are two six-word exception-handler templates:
`St_InstallDmaHandler` compares the first template with the kernel handler
and copies the second over it. The zero at 0x8007A210 follows those templates;
StSetRing starts at 0x8007A214. Neither template is this C function's entry.
The original StClearRing is a separate SDK object, C_002.OBJ.

Stock GCC 2.7.2 gives **85.454544%** against the correctly bounded 44-byte
function. Its sole extra operation is a nop: the second symbolic store is
emitted before the call instead of in its delay slot, shifting subsequent
instructions. The 2.21 assembler option does not resolve this. GCC 2.8.1
with split addresses fills the call delay slot, but changes address registers,
prologue scheduling and the return delay slot. These are experiments, not
permission to change the compiler or assembler.

The production boundary has not been changed or credited as matching C.
Promote only after the complete function matches; preserve the verified SDK
boundary and account for its trailing padding when updating the manifest.
