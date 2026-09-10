# RotAverageNclip3 reconstruction

The source now lives in
[`src/main/psyq/libgte/RotAverageNclip3.c`](../../src/main/psyq/libgte/RotAverageNclip3.c).
It reconstructs the complete 136-byte retail entry at 0x80079384;
`candidate.c` remains the proposal's reproducible compiler input.
It loads three vectors, executes RTPT, stores its flags before NCLIP, and
returns the signed NCLIP MAC0 value. Only a strictly positive value enables
the three SXY stores, IR0 depth store, AVSZ3 and final OTZ store. Rejected
triangles leave those five outputs unchanged; flags are still written.
Output stores preserve retail order, including when pointers coincide.

The source uses the shared GteRotation layout and pe1/gte.h hardware macros.
Control flow and ordinary stores are C; COP2 transfers/commands stay hardware
operations. Explicit hazard slots follow the flag, MAC0 and OTZ reads.
There are no register pins, empty barriers, CPU algorithm instruction
assembly or postpasses.

**Not exact:** stock GCC272 gives 55.14706% (140 bytes versus 136 retail).
GCC281 gives 51.764706% (144 bytes). The maintained candidate uses GCC272.
Stack-argument load timing, branch structure and register allocation still
differ. Production ASM is unchanged; no matching-function credit is claimed.

```sh
tools/scripts/cc.sh proposals/RotAverageNclip3/candidate.c /tmp/rot-average.o
python proposals/RotAverageNclip3/verify_behavior.py /tmp/rot-average.o
```

All **675 cases** pass against the SHA-1-checked retail body: five signed clip
values including both extrema and zero, three projection flag words, three
depths, three OTZ values and five memory layouts. Layouts include separate
outputs, all outputs sharing one word, outputs overlapping input vectors,
reversed overlapping outputs and coincident input vectors. Input vector words
and projected screen words vary deterministically between cases.

The verifier replaces COP2 instructions with NOPs and services their effects
through code hooks. It supplies RTPT, NCLIP and AVSZ3 results rather than
emulating their arithmetic or flags. MFC2/CFC2 results become visible to the
CPU after the following instruction; this catches immediate dependent uses.
It checks input transfers, memory before commands, original projection flags,
conditional store order, complete arena contents, final modeled registers,
stack arguments, returned clip value, SP and canaries. AVSZ3 changes modeled
MAC0 to ensure the original NCLIP return value is retained.

Mutants accepting a zero clip value, returning a boolean, or removing the
flag-read hazard slot fail assertions. These are CPU/interface and transfer-
delay checks, not complete GTE arithmetic, pipeline timing or hardware proof.
Arbitrary overlap with the caller's stack arguments is not covered.
