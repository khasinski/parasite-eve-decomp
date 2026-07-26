# GTE and Assembly Policy

The default representation for game code is C. Assembly is not a substitute
for reconstructing control flow, data layout, arithmetic, or register
allocation.

## Default rule

Do the ordinary reconstruction work first: prototypes, signedness, widths,
structure windows, expression order, control flow, and variable lifetimes.
Do not put ordinary MIPS instructions, branches, pointer arithmetic, bit
packing, or scheduling fixes in inline assembly.

Whole-function assembly is not an acceptable decompilation result. A generated
`INCLUDE_ASM` stub remains the correct representation until the surrounding C
is understood and can replace it without changing the binary.

## GTE and COP2

GTE is a hardware interface, not ordinary scalar C. Game functions may express
their algorithm in C while invoking PSY-Q-style macros that emit a small COP2
operation. Such operations are allowed only through the central
[`include/pe1/gte.h`](../include/pe1/gte.h) interface.

Function bodies must use named operations such as `gte_ldR11R12`, `gte_ldv0`,
`gte_mvmva`, and `gte_stmac`. They must not embed raw `asm volatile` strings.
The macros may contain only the hardware transfer or command and required GTE
hazard spacing. CPU-side work, branch logic, table indexing, and structure
access remain C.

Small GTE macros are not a waiver for arbitrary inline assembly. A new macro
needs all of the following:

- an exact target opcode window;
- a PSY-Q/GTE operation name and documented register effect;
- no ordinary CPU algorithm hidden in the macro;
- a demonstrated source use in a matching candidate; and
- any required `nop`/memory constraint justified by the target sequence.

## Progress and review

The progress report remains intentionally strict: a translation unit with
non-empty GTE macro expansion is not counted as `pure C`. It can be reviewed
as a semantic decompilation candidate, but it is promoted only after its
configured binary range byte-matches.

For each new GTE family, record the target instruction window, scratchpad
layout, macro ownership, first object difference, and the reason scalar C is
insufficient. Do not call a function handwritten merely because it contains
GTE instructions; a normal stack frame and ordinary C control flow are strong
evidence that it originated as C plus PSY-Q macros.
