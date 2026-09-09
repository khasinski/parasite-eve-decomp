# StSetRing boundary candidate

Retail 0x8007A214..0x8007A240 (ROM 0x6AA14..0x6AA40) is a C-like
ring-buffer initializer currently included in St_DmaCompleteCallback's ASM
segment. It stores a0 to D_800C0DC8 and a1 to D_800C20C4, calls StClearRing,
and returns. The preceding 0x8007A210 word is a NOP; entry references should
be audited before assigning a definitive public entry address.

candidate.c was reconstructed directly from the retail instructions.
Stock GCC 2.7.2 produces 0x30 bytes versus the 0x2C-byte body: the store of
a1 precedes jal rather than occupying its delay slot. No source integration
or exact-match claim is made. Separating this C body from the preceding
exception-context assembly is a useful next boundary investigation.

## Compiler/assembler boundary

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
