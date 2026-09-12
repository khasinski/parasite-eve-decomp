# Matrix translation

Production `TransMatrix` at `0x80078C94` matches all **36/36 linked bytes** with
native GCC 2.7.2 and stock MASPSX. The three translation components are copied
to the existing shared `GteMatrix.t` fields and the matrix pointer is returned.
The handwritten `addu` has been replaced by a C pointer assignment.

Four register pins and two empty barriers preserve the load/store order,
pointer-copy position and empty return delay slot. All are recorded in the
debt baseline; no instruction asm remains. The regression in
`tools/tests/test_cos_translation.py` checks the entire linked TU against its
retail SHA-256. No assembler/compiler patches or postpasses are used.
