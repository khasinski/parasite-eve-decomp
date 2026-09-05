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

The projection wrappers `RotTransPers3` and `RotTransPers4` store their result
flags with C through the existing pinned output pointers. The former CPU-ASM
helpers `gte_store_flag_bound` and `gte_store_third_output` were removed.
Their retail stores are at function offsets 0x48 and 0x6C. Each C store is
followed by one empty memory barrier; removing it moves the store into the
return delay slot and moves the depth shift out of that slot. No additional
register pins or hardware instructions are needed for this correction.

`LoadAverageShort0` and `LoadAverageShort12` unpack their two input words and
repack the IR results in C. Only `gte_mfc2_9`, `gte_mfc2_10`, and `gte_mfc2_11`
remain for the result transfers at offsets 0x5C, 0x60 and 0x74. Local volatile
word loads prevent narrowing the second word load to a halfword, and volatile
word stores preserve result order. These qualifications constrain codegen;
they do not identify hardware memory. The left shift uses unsigned arithmetic.
The existing output-pointer pin is still needed; trial final barriers were
removed. Both former packed-short instruction macros were deleted.

`gte_ldv0_word3(source)` packs the low halfwords of two word-stride vector
lanes using C, then emits only `mtc2` and `lwc2`. The source argument is a
pointer lvalue; preserving it across calls and result stores prevents needless
address recomputation. `gte_ldv0_word3_at(vec)` is the one-shot adapter.
Both replace the retired `gte_ldv0_short3` CPU packing window.

The shared implementation retains pins for $12/$13 and two empty barriers.
The pointer barrier prevents folding the address back into stack offsets;
the high-lane barrier is required by the six two-transform initialization
variants. Each pin and barrier was tested for removal. A pointer memory
clobber and low-lane volatile qualifier were removable and are absent.
Only the high-lane load remains volatile to preserve load ordering. The
two draw variants also need an input barrier before setup calls to retain
the original placement of three stack-address calculations.

The subsequent `gte_stir123_column` repair keeps only three `mfc2` operations
in instruction ASM and performs the strided halfword stores in C. Its tied
pointer output preserves the caller-owned address; callers retain that pointer
through related matrix operations to avoid additional copies. Three transfer
output pins remain necessary. Trial adapter pointer pins, volatile stores and
a memory clobber were removable. In the renderer, `gte_getsz3_for_store` owns
only `mfc2` and its hazard slot; the halfword destination store is now C.
The depth and destination pins remain necessary; a trial final barrier was
removed. These fixes repair previously withdrawn eligibility.

## Room Handler D MAC Addresses

The two former private STORE_MAC macros in `RoomLib_HandlerD.inc` performed
ordinary `addiu` address calculations in ASM. Both now use the central
`gte_stmac` operation with C scratchpad offsets 8 and 0x18. For room_m089,
the address instructions are at function offsets 0x210 and 0x3A0. Literal
offsets replace low-half relocations but the linked retail bytes stay identical.

One tied scratch-pointer barrier prevents constant folding to an `ori`; an
entry input barrier with an $18 clobber preserves initialization order without
pinning the scratch pointer. A new $3 pin and tied memory barrier preserve
the matrix component negation after its preceding stores. Two local unsigned
volatile halfword reads preserve the original load widths; these qualifications
constrain codegen and do not claim that scratchpad RAM is MMIO.

The scratch-pointer pin, second address barrier and post-negation input barrier
were removable. Removing the negation pin or its memory clobber breaks exact
comparison. These constraints are in an included template and are not fully
represented by the source-only debt ledger. All 119 linked function ranges
must match retail before treating the family as repaired.

## Room Handler B and C Addresses

HandlerB loads its GTE vector from scratchpad offset 0x20 and stores MAC1..3
at 0x28; HandlerC uses the same input offset and output offset 0x38. Former
private macros combined `addiu` with COP2 transfers. The address calculations
are now C, and only central `gte_ldv0` and `gte_stmac` emit hardware transfers.
For room_m089 the address windows start at 0x23C/0x254 (B) and 0x1C8/0x1E0
(C). Resolving fixed-address relocations to literal offsets preserves the
linked instruction bytes.

Each handler retains an entry input barrier to keep state-pointer setup ahead
of scratchpad initialization. B clobbers $19 and C clobbers $17 at that point;
neither pins the scratch pointer. Removing either entry barrier breaks exact
comparison. HandlerB additionally retains one tied scratch-pointer barrier in
its existing `do/while (0)` helper; removing the barrier or flattening that
block changes codegen. The helper contains C address arithmetic and an empty
barrier, not CPU instruction ASM. Its trial scratch-pointer pin was removable.

HandlerC's trial tied address barrier was removable across all 119 variants.
Its earlier target-vector argument uses an explicit OR with 0x28, valid for
the fixed aligned scratchpad base, to preserve the original `ori`. No new
volatile accesses or register pins were introduced in either family.

The five scene-reset variants reuse the repaired B helper; their private ASM
overrides were removed, as were the two scene_e22 overrides. All 124 B and
119 C linked function ranges match retail after minimization. The retained
barriers total 248 instances across the B variants and 119 across the C
variants, a footprint missed by the source-only debt ledger. These are
repairs of withdrawn semantic credit,
not newly identified algorithms.

## Room CPU Helper Removal

The room arc templates no longer use the raw-word checked-division helper.
Their two divisions are C `/=` expressions. Each leaf caller selects the
existing stock MASPSX `--expand-div` option to reproduce the checks and hazard
spacing. This is not a MASPSX modification. Count and base pins were removable
in both templates; the result pin remains. No new barriers were needed.

`RoomLib_TransformTargetOffset` uses a signed-halfword C load into an `int`,
with no added pin or barrier. The room_m089 random-effect spawner uses C pointer
loads and a C table lookup. It retains two empty barriers: one after the second
sound-pointer load and one after the lookup (with `$2` and memory clobbers).
Removing either changes code generation. Three former pins and the trial first
load barrier were removable. The retired CPU instruction macros are deleted.

## Engine CPU Helper Removal

The engine's `func_800C6148`, `func_800C6584`, and `func_800C6B90` now express
their distance tests as C multiplications and comparisons. Their local
`volatile int delta[3]` preserves two original stack stores; the third function
also preserves a short position buffer. These volatile locals are matching
constraints, not MMIO or independently proven source-level declarations.
No register pins, empty barriers or instruction ASM remain in these three
functions. Keeping the Y halfword in an `int` before narrowing it removes a
trial tied barrier while retaining signed-load code generation.

`func_800C6B20` now uses C position loads and two ordinary calls instead of
an ASM relocation/call window. One result temporary pin remains; three old
pins and all trial barriers were removable. `func_800CC92C` uses a signed-byte
C load after one empty memory barrier, which prevents forwarding the previous
byte store. Its old frame pin remains; removing it changes allocation.

Four further engine units now avoid CPU instruction ASM. `func_800C2E08`
computes the slot-status mask in C, with one tied empty barrier to prevent
the optimizer replacing the arithmetic mask with a conditional assignment.
Its old result pin was removed. `func_800C2FF0` computes width/height decrements
in C with one input-only barrier; both trial pins and the trial tied outputs
were removable. Neither function retains register pins.

`func_800D3F64` needs no explicit ASM `nop`: the stock pipeline supplies the
same load hazard spacing. Its two existing pins and two memory barriers were
tested for removal and remain. `func_800CC878` forms its first output address
and performs the store in C. It retains one pointer pin, one load-order input
barrier clobbering `$4`, and one tied pointer barrier preventing address
folding; removing these changes code generation. No new volatile accesses
were introduced in this group. These are repairs of constrained sources,
not independent evidence that all headers are free of CPU instruction ASM.

`func_800C5EB0` replaces its explicit `move` and unaligned copy macro with C.
Four assignments of halfword-aligned eight-byte records reproduce the
`lwl/lwr/swl/swr` pairs, including their two-word grouping. The local buffer
is now an array of `s16[4]` records; names remain neutral because copy width
and clearing the second halfword do not prove every field's role. The existing
entry pin remains after removal trials. Stock `-fno-strength-reduce` prevents
the loop from acquiring a second derived pointer, so the trial empty pointer
barrier was removable. There are no added barriers or volatile accesses, and
no compiler or MASPSX changes.

The remaining five variants of CC878's address/store window (CC644, CC6F8,
CE3B4, CDE90, CDD0C) now use the same C approach, including C addition in the
last two. Each retains a pointer pin and two empty barriers; individual
removal tests changed code generation. The old RHS pins in CDE90 and CDD0C
were removable. This family adds ten visible barriers and a net three pins,
which must remain explicit matching debt rather than being hidden in a macro.

Seven matrix/scale setup functions (C8270, C8970, C8A88, C9268, C9EA8, CACDC,
CB9F8) now form `&matrix` and copy pointers in C instead of ASM stack-address
instructions. GTE work remains in the existing called routines. Each function
retains two new empty barriers: an input barrier on the loaded scale value
clobbering `$4`, followed by a tied pointer barrier. Removing either breaks
exact comparison. Trial memory clobbers and extra scale inputs on the tied
barriers were removable across all seven variants.

Five variants retain a new `$4` argument pin. C8A88 and CB9F8, which copy an
input matrix, need no new pins; their trial `$17` and `$4` pins and both trial
barriers around the first pointer were removable. Existing index/scale pins
and the existing memory barrier before the scale call remain after removal
tests. This adds five visible pins and fourteen barriers, with no new volatile
accesses, custom toolchain options, or target/layout changes.

## Render Loads And Screen Tint

`Pm_StopAll` now loads `g_PmSlotTable` in C. Declaring the global as a
pointer instead of an array with a first-element macro reproduces both loads
without new constraints. Its existing tied return-value barrier remains;
removing it changes the generated code.

`Render_ApplyScreenTint` now reads the active draw slot and primitive count,
and writes the B/G/R fields, in C. The active-slot declaration is a volatile
scalar, replacing a symbol alias that already exposed volatile reads. Stock
G0 and `-fno-strength-reduce` retain the original address and loop forms.
Four empty barriers retain load ordering and store ordering. Minimization
removed four old pins (tint, both indices, and primitive count), an extra
clobber and input, and the tied operand on the store barrier. One new entry
pointer pin and the two old final-flag pins remain. Removal trials for these
constraints changed the generated code. No CPU-instruction ASM, new volatile
accesses, toolchain modifications, or target/layout edits were introduced.

`Render_StepFade` also replaces its checked signed divide, draw-slot/count
loads, RGB stores, and final flag/geometry writes with C. Stock `--expand-div`
preserves the zero-divisor and signed-overflow checks; default G0 and stock
`-fno-strength-reduce` preserve address and loop forms. Three empty loop
barriers remain after removal tests; no barriers are needed around division
or final writes. Four old pins and all trial pins except a new final-value
`$2` pin were removed. The existing entry `$4` pin remains. A separate
same-symbol store alias prevents address reuse at the final flags write;
merging it back changes code generation. This replaces the old draw-slot
alias, so the number of alias declarations does not increase. No new volatile
accesses, CPU-instruction ASM, or toolchain/target changes are used.

`Render_StepFontLoad` now expresses the byte remainder/nonzero test and
initialization of both search indices in C. One empty tied barrier on the
result and the two initialized values preserves their ordering and the later
byte narrowing. Its trial memory clobber was removable, as was the old count
pin. The other four old pins and the old final memory barrier remain after
removal tests. This adds no pins, symbol aliases, volatile accesses, or flags;
the stock compiler generates the reciprocal multiply for unsigned modulo 10.

`Inv_LoadWayneItemsAsOverride` loads the unsigned item value and writes the
first halfword of its 32-byte category-table record in C. An empty input
barrier on the loaded value clobbering `$2` retains the load-before-offset
schedule. The explicit byte offset is needed; direct two-dimensional indexing
changes allocation. Minimization removed the old index pin, all trial pins,
and two trial barriers. Removing the remaining barrier or its clobber changes
the object. The existing G8 flags remain; there are no new volatile accesses,
aliases, toolchain changes, or target edits.

`Field_GetMapEntry` now uses compiler-generated stack adjustment and a C
pointer assignment instead of explicit CPU ASM. Its unused 16-byte retail
frame is represented by `stack_pad[4]`; the original purpose is unknown,
so this is layout padding, not a recovered structure. One new empty tied
pointer barrier preserves allocation. The three old pins and both old loop
memory barriers remain after removal tests. The draw-slot alias was replaced
by a scalar declaration. No new pins, volatile accesses, flags, toolchain
changes, or target edits were needed.

`Menu_AlignEquipPanels` now uses the existing `MenuWidgetNode` fields `x`
and `popup_node`, typed calls, and C control flow for horizontal alignment.
This removes CPU ASM including raw call/load words and reloc directives.
Stock G8 replaces the explicit GP-relative mode load. Three new pins (target,
current x, and displacement) and one new input-only displacement barrier
remain after removal tests. The trial tied output and `$4` clobber were
removable. The old node-pointer barrier remains. No new volatile accesses,
aliases, toolchain modifications, or target/layout edits were used. The old
m2c macro dependency and untyped declarations are removed from this unit.

`Menu_FindSelectedEquipSlotItem` now uses a C slot store, reload and nested
byte lookup, plus compiler-generated frame setup. The 16-byte unused frame
is explicit padding of unknown original purpose, not a reconstructed struct.
One memory barrier and a distinct same-symbol read alias preserve the reload;
removing either changes the object. A new result pin remains after removal
tests, while the old list pin and unused reload pin were removed. Four other
old pins remain. A direct pointer declaration replaces the old table-pointer
alias, allowing default G0 without per-file flags. No new volatile accesses,
toolchain changes, or target/layout edits are used.

`Inv_InitMaxLevelInventory` now performs the level-table call/indexed load
and unsigned index-range test in C, removing the raw call word and CPU ASM.
A distinct same-symbol level-read alias prevents address caching across the
call. A trial pre-call memory barrier was removable. The two old pins and
old post-HP-store memory barrier remain after removal and merge trials.
No new pins, barriers, volatile accesses, flags, toolchain changes, or target
edits were needed; the additional read alias is explicit source debt.

`Menu_StepItemGrid2` now constructs the memory-card dialog using the existing
`MenuWidgetNode` fields. Ordinary C replaces the node-result copy, callback
address setup, active-list call and text-buffer address loads, including the
old raw call word and relocation directives. Loading the active list before
storing the option cursor allows the compiler to fill the call delay slot.
One new kind-argument pin and one input-only barrier remain; the old suffix
pin and trial label pin, constant barrier and text barriers were removable.
Two same-symbol text-buffer aliases preserve the distinct address reloads.
Removing each remaining pin, barrier or alias changes the object. No new
volatile accesses, flags, toolchain modifications or target/layout edits
were used. The aliases and compiler constraints remain explicit source debt.

`Menu_CreateTwoLineDialog` now selects the minimum-100-pixel dialog width
using C comparisons and repeated text-measure calls, replacing the entire
CPU width-picker window with its raw calls, branches and relocation directives.
The repeated calls are preserved, not folded into cached widths. Two empty
barriers and three C gotos preserve the original branch layout and prevent
layout arithmetic from entering a jump delay slot. Removing either barrier
or using the tested structured conditional alternatives changes the object.
Both old line-pointer pins were removable; trial barrier operands were also
removed. No new pins, aliases, volatile accesses, flags or toolchain/target
changes are used. The adjacent notification draw routine uses the existing
`MenuWidgetNode.selected_base` field and a corrected text-pointer prototype.

`SeqOp_SetPitchLFO` now computes its depth with an unsigned C right shift
and stores it through `AkaoTrack.pitch_lfo_depth`, replacing the handwritten
CPU shift/store pair. One result pin and one input-plus-memory barrier retain
the selector reload ordering. Removing the pin, the barrier, its input or
its memory clobber individually breaks the object match. A trial product
pin was removable. There are no new aliases, volatile accesses, flags,
toolchain modifications or target/layout edits. The other nine functions in
seq_op2.c were already C; their eligibility is restored with this unit.

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
