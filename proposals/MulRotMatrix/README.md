# MulRotMatrix

Production matches **232/232 linked function bytes** at `0x800786E4` and
**240/240 bytes** of the complete original TU, including trailing alignment.
Native GCC 2.7.2 and unmodified MASPSX are used; no postpasses, binary rewriting,
compiler/assembler patches, or ABI-changing call-used flags are involved.

Transforms the three columns of its matrix using the rotation matrix already
held in GTE control registers, and writes the result in place. It does not
replace the GTE rotation control registers.

CPU packing, shifts, masks and stores are C over the shared `GteMatrix` layout.
Each GTE transfer and MVMVA command has its own single-instruction macro.
Translation is preserved. The final IR3 store writes a full word, so its high
half overwrites the matrix padding exactly as in retail.

Ten register pins and eight empty barriers remain, all counted in debt. These
keep three column results live and preserve the original access/packing order.
The `$1` pin holds only the ordinary C high-halfword mask; its lifetime ends
before any assembler-generated address expansion. Seven unnecessary barriers were removed across the pair before integration.

`tools/tests/test_matrix_products.py` compares every linked TU byte with its
retail SHA-256. The existing `verify_behavior.py` passes **1,367 cases** with
scripted GTE results, checking CPU-side packing, preservation and pointer
returns. It does not emulate GTE arithmetic or timing. Exact byte equality is
the production acceptance criterion.

```sh
tools/scripts/cc.sh src/main/psyq/libgte/MulRotMatrix.c /tmp/MulRotMatrix.o
.venv/bin/python proposals/MulRotMatrix/verify_behavior.py /tmp/MulRotMatrix.o
```
