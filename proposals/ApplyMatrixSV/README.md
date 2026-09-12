# ApplyMatrixSV

Production matches **92/92 linked bytes** at `0x80078C34`, using native
GCC 2.7.2 and unmodified MASPSX. The source loads the five packed rotation
words from `GteMatrix`, transfers the input `GteShortVector`, issues MVMVA,
and reads IR1/IR2/IR3 individually and writes `GteShortVector.x/y/z` in C.
The destination pointer is returned and the vector padding is preserved.

Every GTE transfer and command is individually encapsulated in a macro in
`pe1/gte.h`. The hardware hazard nop has its own existing macro. None of these
macros contain normal CPU loads, stores, branches or arithmetic. Pointer
handling and scalar stores are C; no compiler/assembler patches or postpasses
are used. Existing pins and empty barriers are unchanged and counted in debt.

`tools/tests/test_apply_matrix.py` verifies all linked bytes against the retail
SHA-256. Shared type assertions check the matrix and vector ABI sizes.
