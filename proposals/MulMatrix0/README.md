# MulMatrix0

Production matches **268/268 linked function bytes** at `0x800785D4` and
**272/272 bytes** of the complete original TU, including trailing alignment.
Native GCC 2.7.2 and unmodified MASPSX are used; no postpasses, binary rewriting,
compiler/assembler patches, or ABI-changing call-used flags are involved.

Loads the left rotation matrix into GTE control registers, then transforms
the three columns of the right matrix into the output matrix. Inputs are
consumed before output writes, retaining the retail overlap behavior.

CPU packing, shifts, masks and stores are C over the shared `GteMatrix` layout.
Each GTE transfer and MVMVA command has its own single-instruction macro.
Translation is preserved. The final IR3 store writes a full word, so its high
half overwrites the matrix padding exactly as in retail.

Ten register pins and eight empty barriers remain, all counted in debt. These
keep three column results live and preserve the original access/packing order.
The `$1` pin holds only the ordinary C high-halfword mask; its lifetime ends
before any assembler-generated address expansion. Seven unnecessary barriers were removed across the pair before integration.

`tools/tests/test_matrix_products.py` compares every linked TU byte with its
retail SHA-256. The existing `verify_behavior.py` passes **12,303 cases** with
scripted GTE results, checking CPU-side packing, preservation and pointer
returns. It does not emulate GTE arithmetic or timing. Exact byte equality is
the production acceptance criterion.

```sh
tools/scripts/cc.sh src/main/psyq/libgte/MulMatrix0.c /tmp/MulMatrix0.o
.venv/bin/python proposals/MulMatrix0/verify_behavior.py /tmp/MulMatrix0.o
```
