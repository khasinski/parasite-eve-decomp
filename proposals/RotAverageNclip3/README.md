# RotAverageNclip3

Production matches **136/136 linked function bytes** at `0x80079384` and
**144/144 bytes** of the complete TU, including eight alignment bytes.
Native GCC 2.7.2 and unmodified MASPSX produce the match with the default
build flags. No compiler/assembler patches or instruction postpasses are used.

Loads three `GteShortVector` inputs, executes RTPT, stores its flags before
NCLIP, and returns signed MAC0. Only a strictly positive clip enables the
three screen stores, IR0 depth store, AVSZ3 and OTZ store. Rejected triangles
leave those five outputs unchanged. Stores retain the retail overlap order.
Each GTE transfer and command uses an individual single-instruction macro;
CPU loads, stores and control flow are C.

Eight register pins and one empty output barrier are recorded in debt.
The barrier defines a value in architectural `$0`, retaining the original
BGTZ / unconditional BEQ rejection path without an uninitialized C read or
CPU instruction assembly. Volatile pointer parameters retain the original
stack-argument load order. Three redundant barriers were removed before
integration; the zero-register definition remains necessary for source validity.

`tools/tests/test_nclip_projection.py` checks every linked TU byte against
its retail SHA-256, including the branches and trailing alignment.

```sh
tools/scripts/cc.sh src/main/psyq/libgte/RotAverageNclip3.c /tmp/rot-average.o
.venv/bin/python proposals/RotAverageNclip3/verify_behavior.py /tmp/rot-average.o
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
