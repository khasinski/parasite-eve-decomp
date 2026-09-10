# StSetRing and the streaming boundary

`candidate.c` reconstructs `StSetRing` using LIBCD.H's pointer and 32-bit
size parameters. It stores `StRingAddr`, stores `StRingSize`, and calls
`StClearRing`. The SDK names alias the existing retail data symbols.
Their declarations now live in `psyq_cd.h`, shared with the other streaming
ring routines.
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

The old manifest incorrectly folded the function into
`psyq/libgte/St_DmaCompleteCallback`, whose symbol begins at 0x8007A1E0.
The preceding 0x30 bytes are two six-word exception-handler templates:
`St_InstallDmaHandler` compares the first template with the kernel handler
and copies the second over it. The zero at 0x8007A210 follows those templates;
StSetRing starts at 0x8007A214. Neither template is this C function's entry.
The original StClearRing is a separate SDK object, C_002.OBJ.

The unannotated source with stock GCC 2.7.2 gives **85.454544%** against the correctly bounded 44-byte
function. Its sole extra operation is a nop: the second symbolic store is
emitted before the call instead of in its delay slot, shifting subsequent
instructions. The 2.21 assembler option does not resolve this. GCC 2.8.1
with split addresses fills the call delay slot, but changes address registers,
prologue scheduling and the return delay slot. These are experiments, not
permission to change the compiler or assembler.

The manifest now records the verified boundary explicitly:
`psyq/libgte/St_DmaHandlerTemplates` covers 0x8007A1E0..0x8007A214 and
`psyq/libcd/StSetRing` covers the 44-byte callable body through 0x8007A240.
The separate four-byte alignment segment begins there. `St_DmaCompleteCallback`
is classified as a label and `StSetRing` as a function in `sym.main.txt`.
Both ranges remain ASM until their representations match; this boundary fix
does not claim C matching credit.

## Retained earlier compiler investigation


Inspection of raw GCC 2.7.2 output shows `sw $5,D_800C20C4` immediately
before `jal StClearRing`, with no explicit delay-slot instruction. The
current MASPSX/GNU-as pipeline emits the expanded store before the call and
a NOP after it. Retail instead places the expanded store in the call slot.
This points to assembler scheduling as a hypothesis to test against original
ASPSX behavior; it does not justify changing the shared pipeline blindly.
The `-mdebuga` trial retained the same output. `-mno-gas` was rejected by
the installed compiler and is not a valid tested configuration.

## Verified direct-assembler match

The unmodified GCC 2.7.2 assembly, assembled directly with GNU as (reorder
mode, `-EL -G0 -march=r3000 -no-pad-sections`), produces exactly 0x2C bytes.
Linking it with diagnostic.ld at 0x8007A214 and comparing .text to all eleven
retail instruction words gives **44/44 bytes identical**. GNU as moves the
second expanded store into the jal delay slot, preserving the retail return
sequence. No instruction editing is involved.

This is a verified isolated body match, not production integration: the normal
MASPSX pipeline still differs, and the public entry boundary still needs audit.
The direct-assembler result identifies a concrete pipeline compatibility issue
for this candidate rather than a need to distort its C semantics.

## Current stock-pipeline candidate

The saved candidate now selects GCC281 with `-fcall-used-$1` and
`-fno-schedule-insns2`. A 16-combination sweep of the two scheduling flags,
delayed-branch optimization and expensive optimizations found this sufficient
to reproduce the retail prologue and entire call sequence through stock
MASPSX. Linked .text is 44 bytes with six differing bytes, all in the epilogue:
retail restores sp before jr and has a NOP delay slot; this candidate places
the NOP before jr and restores sp in its delay slot. No pins or barriers.
Earlier GCC272/direct-as observations above describe the original unannotated
source; the current production-path candidate is still not an exact match.

The SDK object comparison above now resolves the earlier entry-address uncertainty.
