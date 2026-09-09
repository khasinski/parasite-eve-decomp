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
