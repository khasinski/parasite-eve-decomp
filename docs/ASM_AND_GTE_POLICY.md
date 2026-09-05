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

Compiler register variables such as `register T value asm("$N")` and empty asm
barriers are allowed when they reproduce the original allocator or scheduling
result while leaving the function's control flow and behavior in C. They are
source-level compiler constraints, not inline instruction sequences. Keep them
as narrow and few as practical. After obtaining an exact match, explicitly try
to remove each pin and barrier and retain only those whose removal breaks the
match. The debt ratchet records the remainder.

The compiler and MASPSX are hard boundaries. Final builds must use the
unmodified stock compiler and the unmodified, pinned upstream MASPSX revision:
no local patches, diagnostic builds, postpasses, or options absent from those
stock tools. A register pin or barrier is preferable to modifying either tool.
If stock C with allowed constraints does not match, the function remains an
assembly subsegment until its source reconstruction is corrected.

For small-data loads followed by symbolic stores, upstream MASPSX's
`--aspsx-version=2.21 --dont-expand-li` reproduces the required load hazard
spacing while leaving `li` expansion to GNU as. The source markers
`ASPSX_VERSION: 2.21` and `MASPSX_FLAGS: --dont-expand-li` select these stock
options through `cc.sh`. `Task_AllocNode`, `Menu_CreateInvSwapView`, and
`Menu_StepSaveSelect` match with these settings after removing handwritten
`nop` instructions. These flags are per-unit choices, not a global default.

Inline instructions are allowed only for individual GTE/COP2 hardware
operations, or in functions with concrete evidence that the original source
was assembly. Put each permitted instruction sequence behind a small, named,
central macro and isolate only the instruction window that C cannot express.
Do not hide surrounding CPU control flow, arithmetic, loads, or stores in that
macro. Evidence for an originally assembled function must be recorded beside
the implementation or in this document.

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

## PSY-Q BIOS trampolines

The 12-byte A0/B0/C0 veneers shipped in the PsyQ libraries are proven assembled
stubs. Every instance has the canonical three-instruction BIOS ABI shape: load
the BIOS table address into `$t2`, jump through `$t2`, and load the service
selector into `$t1` in the delay slot. They have no C prologue, epilogue, stack
frame, or ordinary function body, and preserve the caller's argument and return
registers across the tail call.

These veneers may use `PSYQ_BIOS_TRAMPOLINE` from
`include/pe1/psyq_bios.h`. The macro owns exactly those three instructions and
must not grow into a general inline-assembly escape hatch. The table and
selector constants must be copied from the retail veneer and each generated
object must still pass normal score, section, linked-range, and executable
checks.

## LZCS / LZCR

`gte_ldlzcs`, `gte_stlzcr`, and `gte_getlzcr` are owned by
`include/pe1/gte.h`. They represent the GTE leading-zero count data-register
pair: `mtc2 value,$30` writes LZCS; `swc2 $31,0(ptr)` stores LZCR; and
`gte_getlzcr` transfers LZCR to a general-purpose register after its two
required hazard slots. Scalar C cannot express these hardware transfers.

The initial target windows are `Gte_StoreTableEntry` at `0x8003EACC`, the
type-3 branch of `Task_DispatchCmd` at `0x80013188`, and `Gte_ISqrt` at
`0x80078004`. The first two store LZCR through a normal stack or argument
pointer; `Gte_ISqrt`, `Gte_VectorOp`, and `Gte_MatrixOp` read it after two nop
hazard slots. No scratchpad layout is involved. The first remaining difference
in `Gte_StoreTableEntry` is an 8-byte compiler frame for the C local receiving
LZCR, not the COP2 window itself. The dispatcher still needs source-shape work
for argument-load timing and branch sharing. These macros contain only the
documented COP2 transfers and a memory constraint for the LZCR store.

## NCLIP

`gte_ldsxy0`, `gte_ldsxy1`, `gte_ldsxy2`, `gte_nclip`, and `gte_stmac0` are
the central interface for screen-space triangle winding. They load SXY0..2,
run the documented NCLIP command, and store MAC0. `gte_nclip` owns the two
required hazard slots before opcode `0x4B400006`; callers retain all culling
and primitive-building logic in C. Initial users are the NCLIP helpers in
`Entity_UpdateAndRender`, `Render_DecompressAnimFrame`, and
`Render_DrawTexturedQuads`. No scratchpad memory is involved.

## RTV0TR / IR1..3

`gte_ldv0`, `gte_rtv0tr`, `gte_rtv0tr_mac`, `gte_getir1` through
`gte_getir3`, and `gte_stir123` express the PSY-Q vector transform used for
object bounds, vertices, and matrix translations. The target windows load V0
from two words, execute RTV0TR (`0x4A480012`), and read IR1..3 or store
IR1..3/MAC1..3. The IR-read window has one hazard slot; the MAC-store windows
have two. The C caller owns source and destination layout; the macros only
move COP2 state. Initial users include `Render_DrawObjectAlt`,
`Render_DrawObjectVariant`, `Render_DecompressAnimFrame`,
`Render_TransformMorphVertices`, `Render_SetupBoneTransforms`, and
`Task_SetGteMatrix`.

## RTIR12

`gte_ldir123` loads the current vector into IR1..3 and `gte_rtir12` applies
the documented rotation-only MVMVA form (`0x4A49E012`) after one hazard slot.
IR results are read through the existing `gte_getir*` macros; matrix layout
and destination stores remain ordinary C. Initial users are the matrix-axis
helpers in `Render_DecompressAnimFrame`, `Render_SetupBoneTransforms`,
`Render_TransformMorphVertices`, `Render_TransformSkinnedVertices`,
`Render_TransformVertices`, and `Task_SetGteMatrix`.

## LoadAverage byte results

`LoadAverageByte` and `LoadAverageCol` read MAC1..2 or MAC1..3 with
`gte_getmac12_staged` and `gte_getmac123_staged`. These macros contain only
two or three `mfc2` instructions. Their input constraints preserve output and
shift setup before the transfer; shifts and byte stores live in the C callers.
The target transfer windows start at function offsets 0x40 and 0x50,
respectively. A final empty memory barrier preserves the stores before the
return rather than moving the last store into its delay slot. Removal fails
exact comparison, as does removing the output-pointer pin. Trial shift
barriers were removable and are not retained. The output pin already existed
in the removed helper; moving it into C makes this debt visible to the ledger.

## OP / Outer Product

`gte_pushrotcol0`, `gte_ldopv1`, `gte_ldopv`, `gte_op0`, `gte_op12`, and
`gte_poprotcol0` express the PSY-Q outer-product sequence. OP temporarily
uses the R11/R22/R33 diagonal entries as its control inputs, so the wrappers
at `OuterProduct12` (`0x80079178`) and `OuterProduct0` (`0x800791D0`) preserve
those entries, load the first vector into the control registers, load the
second vector into IR1..3, execute OP, store MAC1..3, and restore the original
matrix entries. The only CPU loads in `gte_ldopv1` are the required bridge from
memory into COP2 control registers; all arithmetic and destination handling
remain in the C caller.

## RTPS / RTPT

`gte_rtps` projects V0 after two hazard slots and exposes SXY2/SZ3 through
`gte_stsxy2` and `gte_getsz3`. `gte_ldv012` and `gte_rtpt` load and project
three vectors, with one required hazard slot, then store SXY0..2 and SZ1..3.
The operations are the documented GTE perspective transforms (`0x4A180001` and
`0x4A280030`); clipping, depth selection, and output-buffer management stay in
C. Initial users are `Render_DecompressAnimFrame`, `Render_DrawObjectVariant`,
and `Render_TransformMorphVertices`.
