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

`CdRom_RestartSeek` selects stock GCC281's `-fcall-used-$1` together with
`-fno-expensive-optimizations` through a per-file `CC1_FLAGS` marker. This makes
AT available for the target's split-address store; a single v1 pin keeps the
later -1 argument out of AT. No instruction ASM or barrier remains. This is
not permission to change the global register convention: inspect the complete
generated function for assembler scratch collisions and require its exact
match and full retail SHA before accepting such a per-file choice. `cc.sh`
only forwards the stock option; the compiler and MASPSX remain unchanged.

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

`SeqOp_NoteOnWithPitchSlide` now searches the 24-voice mask in C instead
of raw CPU words. The track accesses use existing `AkaoTrack` fields.
One new valid-mask pin and one backward C goto preserve the target loop;
structured loop trials duplicated or rotated its occupancy test. Four trial
barriers and a second goto were removable. The old three pins and entry
barrier remain after individual removal tests, as does the new mask pin.
No new aliases, volatile accesses, compiler flags, toolchain modifications
or target/layout changes were used. The preexisting stock checked-divide
profile is unchanged. The byte shift now explicitly uses an unsigned input
before sign extension, without changing the generated instructions.

## GPU Rectangle Diagnostic Address

`checkRECT` uses the fixed USA message address `0x80011898`, formed in C
as page `0x80010000` plus string offset `0x1898`. One tied pointer barrier
keeps those operations separate so stock GCC281 and MASPSX place the offset
addition in the jump delay slot. This is a USA-specific address calculation,
not an inferred portable string-pool structure; moving the message requires
updating this calculation. No instructions are emitted by the barrier.
The direct symbolic address compiled to an extra delay-slot nop (score 160).
Removing the remaining height argument pin scores 350 versus 10 for the raw
matching object; those 10 points are only resolved-symbol versus literal
address notation. Linked score is 0/7100 and the full retail SHA matches.
No compiler/MASPSX changes, instruction ASM, or target changes are involved.
The unlinked objdiff report remains at 99.85915% for this unit because it
compares the synthetic target relocations to literal addresses. Consequently
the report does not credit this function or its 284 bytes as matched. The
progress pipeline is unchanged; linked-byte verification is not substituted
for its existing relocation-sensitive metric.

## GPU Clear Packet Address

`Gpu_DmaVramTransfer` (the SDK ClearImage packet builder) similarly forms
its fixed USA packet address `0x800A3300` as page plus offset in C, with one
empty tied barrier. A second empty barrier preserves the zero return value
placement; removing it scores 297 versus the matching object's raw 10.
There are no pins or instruction ASM. Linked score is 0/14000 for 560 bytes
and the full retail SHA matches. Tests cover aligned fill, unaligned sprite
and state restoration packets, dimensions and colors. This fixed-address
form has the same unlinked relocation-report limitation as `checkRECT`;
it does not imply portable packet storage or justify changing report credit.

## GPU Reset State And Dimensions

`Render_InitEntityPool` is the legacy name of the SDK ResetGraph routine.
Its C reconstruction clears a 128-byte GPU state, resets callbacks and the
queue, loads width/height from four-byte table entries, and invalidates the
two cached environments. Other modes use the dispatch table's soft reset.
Three register pins and four empty barriers preserve argument setup and
indexed width access on stock GCC281/MASPSX. The width read is volatile to
retain its order relative to the volatile GPU-variant read. A fifth barrier
after the page/index sum was removable. Diagnostic argument pins were also
removed; removing the offset or cache pointer pin broke the match.
The USA state address `0x8009574C` and width-table address `0x800957CC` are
formed from a fixed page plus offsets in C. These are version-specific
address calculations, not relocatable declarations. Raw score20 reflects
the two address pairs; linked score0/9300 covers all 372 bytes. Full retail
SHA matches. No instructions in ASM, target edits or toolchain changes.
As with the preceding GPU functions, unlinked objdiff need not grant matched
credit for a literal-versus-relocation difference; its rules are unchanged.

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

## Room 350 swept-area emitter

`RoomEffect_SweptAreaEmitter` (`func_80198400`) keeps particle emission,
endpoint publication, quad construction, ownership flags and rendering
configuration in C. Only NCLIP and its COP2 register transfers use the existing
GTE macros; no instruction macros or toolchain modifications were added.

Four snapshot-pointer pins, five empty barriers and two same-symbol field-store
aliases remain after removal tests. The aliases expose the first halfword of
the complete eight-byte endpoint arrays without changing relocation names.
Three ordered coordinate reads and the endpoint field writes remain volatile
as matching constraints, not as claims of MMIO. Five other volatile qualifiers,
five pins and three barriers were removed after reaching score zero.
The final loop uses ordinary array indexing and an explicit index increment;
the experimental zero-result pin/barrier and staged area pointer are gone.
No fabricated frame locals or instruction scheduling postpass is used.

## Room 318 falling palette sprite

`RoomEffect_FallingPaletteSprite` (`func_80192DA0`) uses C for 16-bit motion,
the 24-frame completion test, palette selection and renderer calls. Three
register pins and two empty input-only barriers remain as matching constraints.
Ordered particle reads are volatile for matching, not as a claim of MMIO;
the motion stores are ordinary C stores. There is no instruction ASM.

Post-zero removal tests eliminated speed, kind and Z pins and the final
constant/Z barrier. Removing Y, delta or the comparison-constant pin loses
the match, as does removing either retained barrier or all volatile reads.
The eight-byte helper output and position vector are used locals, not padding.

## Room 273 sized table sprite

`RoomEffect_SizedTableSprite` (`func_80197230`) retains three register pins
and two empty input-only barriers, one with a memory clobber. They preserve
the first sample capture, renderer-state read and size-register lifetime.
Both table accesses remain full-word C reads; dimensions and brightness use
their low signed halfwords. No instruction ASM or volatile access is needed.

After score zero, removing sample, comparison-constant or size pins yields
scores 10, 35 or 38. Removing the first barrier gives 885, its memory clobber
680, and the size barrier 295. These are stock-compiler matching constraints,
not hardware semantics. Helper mutation tests cover the pre-call size capture
and post-call brightness read with the original captured frame index.

## Room 273 double-sized table sprite

`RoomEffect_DoubleSizedTableSprite` (`func_80194470`) retains one comparison
constant pin and two empty input-only barriers (one with a memory clobber).
Capturing the first offset before saving the frame reproduces the original
instruction order. Feeding that saved frame to the size barrier resolves
allocation without pinning the frame or size registers.

After matching, position and first-frame pins were removed. Removing the
remaining pin yields715; removing the first barrier38, the second588, or its
memory clobber390. The first table access is a signed halfword, the second a
full word with its low signed halfword used for brightness. Neither is ASM.

## Room 273 dual-sample sprite

`RoomEffect_DualSampleSprite` (`func_80199950`) retains one comparison-constant
pin and one empty tied-output barrier for size and shade with a memory clobber.
Both samples are full-word reads captured before GetClut; the position pointer
is read after that call. No instruction ASM or volatile access is used.

An early frame/pointer barrier was removed after score zero. Removing the pin
gave815; removing the memory clobber with the early barrier present gave530.
Eight explicitly unknown frame bytes are layout scaffolding, not a recovered
semantic structure. Removing them from the minimized variant gave74.

## Room 273 delayed rising sprite

`RoomEffect_DelayedRisingSprite` (`func_80199568`) retains a speed-register pin,
one empty barrier tying the 16-bit Y value to the updated speed, and three
volatile reads. The duplicate speed reads and their order reproduce the
target; volatile is a matching constraint, not a claim of MMIO. Stores are
ordinary C, and unsigned motion fields express modulo-16-bit storage only.

Post-zero removal tests: pin525, volatile Y20, barrier160. Narrowing the local
Y to unsigned short eliminated redundant masking without an instruction ASM
workaround. Rendering remains C; there is no padding or instruction ASM.

## Room 273 paired emitter

`RoomEffect_PairedEmitter` (`func_801993F0`) retains one empty tied-pointer
barrier inside the two-record loop. It prevents induction-variable splitting;
removing it after score zero gives562. No pins, volatile accesses, frame
padding or instruction ASM are used.

The partial state view at D_8019AF84 contains two eight-byte records and a
control byte at offset30; intervening bytes have unknown purpose. The anchor
uses the enclosing object's byte representation. Source record+6 is flags;
the allocated record+6 is only known to be cleared, not semantically flags.
Relocation spelling AF84+30 versus AFA2 gives raw score10; resolving addresses
gives upstream score0/9400, with the target verified against original bytes.

## Room 273 gated pair emitter

`RoomEffect_GatedPairEmitter` (`func_80198CD4`) copies two eight-byte records
using ordinary C struct assignment, producing the original unaligned word
loads/stores without instruction ASM. Its partial state view starts at AF74;
the control byte is offset46 and intervening bytes have unknown purpose.

One empty palette-input barrier clobbers memory and v0 to preserve capture
and constant scheduling. Only the two globals with repeated stores remain
volatile as matching constraints, not MMIO claims. Index/palette pins and
all other volatile qualifiers were removed after reaching linked score zero.
Removing the memory clobber with the palette pin present gave raw625.
There is no frame padding. AF74+46 versus AFA2 gives raw10, linked0/11200.

## Room 273 ground-layer sprite

`RoomEffect_GroundLayerSprite` (`func_80198B1C`) retains one comparison-constant
pin and one empty shade/size input barrier with a memory clobber. An earlier
sample-capture barrier was removed after matching. Removing the remaining
pin gives715 and removing the remaining barrier1305 on upstream scoring.

The full vector copy and both render calls stay in C. Samples and signed
shade are retained across helpers; only the local Y is overwritten from the
ground-height global between layers. No volatile, frame padding or instruction
ASM is required. The stack frame follows used locals and outgoing arguments.

## Room 273 growing rising sprite

`RoomEffect_GrowingRisingSprite` (`func_801974DC`) retains one speed pin,
three empty barriers and three volatile motion reads. These are matching
constraints, not MMIO. The eight unknown stack bytes represent frame layout,
not a reconstructed semantic field. No instruction ASM is used.

After score zero, the particle pin, palette barrier and sample barrier memory
clobber were removed. Removal probes for the early offset barrier, sample
barrier, motion barrier, speed pin, padding and all volatile reads scored
215, 695, 160, 525, 90 and 125 respectively. Samples and frame are captured
before GetClut; scale is read after it. Dimensions remain full integers.

## Room 273 position emitter

`RoomEffect_PositionEmitter` (`func_80197A48`) uses one empty palette-input
barrier with memory and v0 clobbers, without pins, padding or instruction ASM.
The Y/Z source reads and X/Y output stores are volatile matching constraints,
not MMIO; the X source read and Z output store are ordinary accesses.
The repeated writes to 3376/3378 also retain volatile qualifiers.

After score zero, X source volatility was removed. Removing Y/Z source
volatility scores440/840; removing X/Y store volatility365/340. Removing the
barrier scores805, its memory clobber1270, or its v0 clobber90. The output's
fourth halfword is unknown and untouched. Flags are observed, not consumed.

## Room 273 timed rising emitter

`RoomEffect_TimedRisingEmitter` (`func_80197648`) has no pins, barriers,
instruction ASM or padding. The inherited palette barrier was removed after
score zero. Y/Z source reads, X/Y/speed output stores and repeated writes
to 3376/3378 remain volatile matching constraints, not MMIO declarations.
Removing Y/Z read qualifiers scores495/975, X/Y store qualifiers495/570;
the speed-store removal scored315 before the palette barrier was removed.

The signed halfword counter is postdecremented only when emission is active.
Allocation failure preserves that decrement; success resets it to two.
Coordinates are read after allocation, Y is offset by -256, and speed is zero.

## Room 273 periodic pointer pair

`RoomEffect_PeriodicPointerPair` (`func_80199A90`) keeps one empty palette
input barrier with memory/v0 clobbers and volatile repeated 3376/3378 writes.
There are no pins, padding or instruction ASM. Initializing the loop counter
before the position pointer reproduces allocation without register constraints.
After score zero, removing the barrier scores1405, its v0 clobber90, or its
memory clobber1455. These constraints express matching, not hardware semantics.

Each allocated four-byte effect holds a live pointer to one of two eight-byte
position records, not copied coordinates. A failed allocation stops the loop
without undoing earlier allocations. The completion latch is independent of
the eight-frame emission interval.

## Room 273 phased pointer sprite

`RoomEffect_PhasedPointerSprite` (`func_8019A4CC`) retains one frame-register
pin and eight explicitly unknown frame-layout bytes. Removing these after
score zero gives471 and26 respectively. There are no barriers, volatile
accesses or instruction ASM. Unknown stack bytes are not semantic fields.

The used eight-byte parameter block contains zero, the object's halfword at
offset58, a wrapping phase and one. Only the context pointer at offset8 and
that object halfword are identified; surrounding structure fields remain
unknown. Size and shade are full integers passed to the renderer.

## Room 273 batch rising emitter

`RoomEffect_BatchRisingEmitter` (`func_8019706C`) retains one tied-index
barrier consuming the old X pointer, and eight unknown frame-layout bytes.
Removing the barrier after linked score zero gives1276; removing the frame
bytes gives90. No pins or instruction ASM remain. Only repeated renderer
configuration writes are volatile. A plain local byte offset reproduces the
target's address addition without constraining a register.

The partial batch has coordinate arrays at offsets0/24/48, a signed count
at72 and stop byte93. Twelve elements are inferred from spacing, not a proven
runtime bound. The count is re-read after allocation and cleared after the
loop even on allocation failure; the stop path leaves it untouched. Equivalent
AF0C-relative relocations are verified by linked upstream score zero and full
retail SHA, not treated as zero by changing the scorer or target.

## Room 273 colored rising sprite

`RoomEffect_ColoredRisingSprite` (`func_80195E10`) retains two register pins,
one size-input memory barrier and two volatile speed reads. No instruction
ASM or frame padding is used. After score zero, removing the motion barrier
and volatile Y read still matches; removing the speed pin then gives320.
Removing both volatile speed reads gives125; restoring only the second gives20.
Removing the render barrier's memory clobber gives1025. Earlier removal of
the special-kind pin gives330. Initial sample pin and barrier were also removed.

The four-byte color record is copied with ordinary C struct assignment;
its fourth byte remains unnamed. Size and color are captured before GetClut,
whereas shade is sampled afterward using the saved frame index. Dimensions
remain full integers, and the motion halfwords wrap on assignment.

## Room 273 threshold pools

`RoomEffect_ThresholdPools` (`func_801949EC`) uses one input-only empty
barrier for the first pool's returned size. Without it, upstream score is60;
pinning that size does not help. The memory clobber was removed after zero
without losing the match. There are no pins, volatile accesses, padding or
instruction ASM in this function.

The partial state identifies a kind byte at14 and halfwords at22/26. The
predicate is kind9, signed value22 >=4, signed value26 <4; their meaning as
current/previous values is not proven. Remaining bytes are explicitly unknown.
The scene's pointer at0x238 is read after allocation; position is pointer+20.
Allocation failure still updates the secondary pool. A nonzero stop byte
returns2 without touching either pool; initialization returns both pool sizes.

## Room 273 threshold pair

`RoomEffect_ThresholdPair` (`func_80194284`) retains one empty memory barrier
between palette selection and the final renderer configuration stores.
Without it score is545; consuming the palette as an input gives zero, but
removing that input still matches. There are no pins, padding or instruction
ASM. Only repeated configuration writes at3376/3378 use volatile.

Two eight-byte templates are copied with ordinary C struct assignment, then
their halfword at2 receives the same random angular offset, wrapping to16
bits. Other bytes remain unnamed and are copied intact. Allocation failure
stops without undoing an earlier copy. The signed predicate uses kind9,
value22 >0 and value26 <=0; current/previous naming remains unproven.

## Room 273 threshold burst

`RoomEffect_ThresholdBurst` (`func_801945A8`) retains one empty barrier with
memory and v0 clobbers. Removing v0 gives90; removing memory gives1535.
No pins, padding or instruction ASM are used. Only repeated3376/3378 writes
are volatile. The state predicate uses kind9, value22 >=2 and signed value26
<2. It rearms four emissions and clears the delay, even during an active burst.

The signed delay is post-decremented whenever the remaining count is nonzero;
allocation is attempted when the old delay is nonpositive. Failure retains
that decrement and does not consume an emission. Success copies three player
coordinates into halfwords, leaves the fourth halfword untouched, resets
delay to2 and decrements the count. Player data is resolved after allocation.

## Room 273 triple-layer sprite

`RoomEffect_TripleLayerSprite` (`func_801977F8`, 592 bytes) retains three
register pins: initial counter v0, saved frame s6 and packed sample v1.
Removing them individually from the matching candidate gives upstream
decomp.me scores 20, 1920 and 390 respectively. An empty barrier consuming
the saved frame and clobbering memory follows the final local vector writes;
removing either its input or memory clobber gives483. Three earlier input-only
barriers were unnecessary and removed. No padding or instruction ASM is used.

The callback copies all eight position bytes, renders a sprite and then two
additional layers. The packed animation sample is reloaded after the first
draw using the saved frame, not the potentially changed global counter.
The last draw retains the second sample's shade and reads the floor height
after the second draw. Rotation and position are passed by address, so helper
mutations of fields not subsequently overwritten remain observable.

## Room 273 sampled-layer emitter

`RoomEffect_SampledLayerEmitter` (`func_801933A0`, 412 bytes) retains one
empty memory barrier after copying the first position halfword. Without it,
the tested address-local candidate scores1440 in upstream decomp.me scoring.
The address calculation initially used a v0 pin; removing it preserves zero.
There are no remaining pins, volatile accesses, padding or instruction ASM.

The pool uses twelve-byte records and callback8019320C. A successful emission
copies XYZ from the current state's transform at594/598/59C, then writes
three parameter bytes at8/9/A; halfword6 and byteB are untouched. The parameter
semantics remain unnamed. The frame counter is reread after allocation and
again after the first two parameter writes, rather than cached. The transform
chain is also resolved after allocation.

## Room 273 scaled-layer pair

`RoomEffect_ScaledLayerPair` (`func_8019353C`, 436 bytes) has no pins,
barriers or instruction ASM. An initial sample v1 pin was removed after
score zero, preserving the match. Eight bytes of unknown frame layout remain;
removing them from the pinned candidate gives upstream decomp.me score90.
They are layout scaffolding, not reconstructed semantic fields.

The initial scale is the signed halfword at input6. The renderer receives a
local copy of all eight vector bytes and the original parameter pointer at8.
Its two iterations retain signed sixteen-bit size and shade: the size grows
by3/2 with truncation toward zero, while shade halves arithmetically. Both
samples are read before either draw; subsequent helper mutations are not
grounds for reloading them. Local vector mutations are retained except for
position.y, overwritten from942EC after each draw.

## Room 273 scaled-layer emitter

`RoomEffect_ScaledLayerEmitter` (`func_801936F0`, 384 bytes) retains a single
empty memory barrier after the X store and eight unknown frame bytes. Removing
the barrier in the sample-input-barrier candidate gives upstream decomp.me
score1070. The sample input barrier was subsequently removed with score zero;
the remaining int sample local preserves LH. Removing the frame bytes from
the minimized candidate gives26. No pins, volatile or instruction ASM remain.

The twelve-byte output's halfword6 is a scale, confirmed by callback8019353C.
It receives the second half of the indexed animation sample. Parameter bytes
8/9/A receive40, the first sample scaled by40/4096, and0; byteB is untouched.
The phase counter is reread after byte8 is written. The coordinate transform
and animation counter are resolved after allocation, not before it.

## Room 273 latched-player emitter

`RoomEffect_LatchedPlayerEmitter` (`func_801939B4`, 424 bytes) retains one
empty barrier clobbering memory and v0. With a palette input but without v0
the candidate scores90; removing the palette input after zero preserves zero.
Removing memory from the minimized candidate gives1420. There are no pins,
padding or instruction ASM. Only repeated3376/3378 writes are volatile.

Mode0 clears the latch and initializes one four-byte pointer record. Mode1
returns2 at counter>=20 or nonzero latch; otherwise player byte14 must be<4
for allocation. Success latches and resolves the current player transform
after allocation, storing its position address at transform+20. Failure does
not latch. Mode2 writes renderer configuration only when the latch is nonzero.

## Room 273 threshold triple

`RoomEffect_ThresholdTriple` (`func_80193F30`, 504 bytes) retains two empty
barriers: one consumes the first pool size, the other clobbers memory after
copying X. Removing them individually gives upstream decomp.me scores220
and635. No pins, padding, volatile or instruction ASM are used.

It creates a sixteen-byte primary pool and four-byte secondary pointer pool.
The kind9, value22>0, signed value26<=0 predicate emits up to three primary
records, with phase halfwords0/384/768. Only the first receives a secondary
pointer record. Primary allocation failure stops further emission; original
code assumes the secondary allocation succeeds and dereferences it unchecked.
The captured state pointer survives helper calls, but its transform and the
primary pool are reread. Secondary update runs even on a false predicate or
primary failure; the stop byte suppresses it, but not mode2 rendering.

## Room 273 threshold model

`RoomEffect_ThresholdModel` (`func_80193CB8`, 632 bytes) retains five pins
(shifted table pointer a0, table base a2, packed sample v0, asset address s1,
special kind v1) and four empty barriers (index, scale-store ordering, paired
phase/parameter, page). No instruction ASM, volatile or frame padding is used.
From the initial zero candidate, removing table/sample/asset/special-kind pins
gave25/65/160/550; removing the four barriers gave65/335/305/1065. Frame and
wave-pointer pins were removed together with zero preserved. Removing those
and the shifted-pointer pin together gave975, so that pin remains.

The sixteen-byte record contains XYZ, phase, XZ scale, Y scale, a renderer
parameter and one still-unknown halfword. Update sets the scales from paired
animation samples, increments phase256 and derives parameter12 with signed
shift6. Render builds a Y-rotation matrix, applies a copied sixteen-byte scale
vector, then reads current translation fields. Helper calls may change the
asset pointer, phase or scale; the original rereads are retained. Matrix pad
is not initialized by the original and is not given an invented value.

## Room 273 paired batch emitter

`RoomEffect_PairedBatchEmitter` (`func_80195BD0`, 576 bytes) retains three
pins (stop address v1, batch base s3, byte offset a0), four empty barriers
(first pool size, offset before coordinate reads, tied offset between Y/Z,
index after count read) and eight bytes of unknown frame layout. Removing
the pins independently scored 25/15/405; removing the barriers scored
60/160/225/120; removing frame padding scored 90. Two other barriers were
removed together with score zero preserved. No instruction ASM is used.

The buffer at 0x8019AEB8 has eight halfwords each for X, Y and Z, a signed
count at offset 0x30 and a stop byte at 0x40. The stop and count symbols
are aliases within this buffer, not separate allocations. Fourteen bytes
between count and stop remain unknown. A twelve-byte primary record stores
XYZ, size and phase; its final halfword is untouched. Each successful primary
allocation is followed by a secondary pointer-record allocation, unchecked
in the original. The original therefore requires secondary capacity.
Coordinates and count are read after those helpers, and pool globals are
reread. Primary failure stops emission, then clears the pending count and
updates the secondary pool. A set stop byte still updates that pool and
returns 2 without clearing count. Render updates the secondary pool before
reading palette state. Repeated writes to 0x800F3376/78 remain volatile.

## Room 273 periodic dual pools

`RoomEffect_PeriodicDualPools` (`func_801960F4`, 596 bytes) retains two empty
barriers (first pool size input and palette memory/v0 clobber), eight bytes
of unknown frame layout and a volatile second timer read. No pins or
instruction ASM remain. Removing the frame bytes scored 58; removing the
timer volatile read scored 358; removing the two barriers scored 60/1315.
Removing only the palette v0/memory clobber scored 90/1430. The stop-address
pin and palette input operand were removed together with score zero retained.

Both pools contain four-byte pointer records and have capacity six. For
state kind 11, timer values less than 35 whose low three bits are zero emit
up to two references into each pool. Negative multiples of eight also pass;
the original does not require a nonnegative timer. The first loop uses the
position base 0x5C bytes before the stop symbol; the second uses its alias
0x8019AE9C. Each position occupies eight bytes. A primary allocation failure
does not suppress the independent secondary loop. Pool globals are reread
after helpers; the predicate and stop flag are not reevaluated during loops.
Kind 11 always updates the secondary pool after the predicate, while other
kinds and an initial stop flag do not. Render runs regardless of that flag
and reads palette state after its helper. Remaining buffer fields are unknown.

## Psy-Q CD command retry wrapper

`func_8007A4D0` in `psyq/libcd/command_retry.c` matches all 316 relocated
bytes with stock GCC 2.8.1 and stock MASPSX. Three pins retain the saved
callback, return value, and comparison constant. Removing them independently
from the final zero candidate causes 15, 35, and 2 differing bytes.
Five other pins were removed while retaining the match, including two after
replacing goto edges with a do/while loop and removing the debug-output flag.

Five empty read/write barriers retain the comparison constant, the three
arguments to the preliminary status command, and the final failure value.
Removing them independently from the final zero candidate causes 4, 9, 3,
1, and 34 differing bytes. The argument barriers prevent zero arguments from
sharing a register and preserve their original order. No instruction assembly
or tool changes are used. The wrapper attempts the command four times,
restoring the completion callback before the actual command and after
exhaustion. The remaining callback representation is opaque integer storage;
this function does not call it.
The third command argument is a byte-result pointer: `CD_cw` preserves it
in s6 and writes response bytes through it at 0x8007B90C. Using `u8 *`
in the shared prototypes and wrapper preserves the match.

## Psy-Q last CD position

`CdLastPos` is now plain C returning `&g_CdLastPos`, whose existing linker
symbol is at 0x8009AFD0. The object is a four-byte `CdlLOC`. Stock GCC 2.8.1
places the low-half address addition in the return delay slot and matches
all twelve bytes without constraints. This removes the previous synthetic
64 KiB page type, global volatile register pointer, and v0 pin. No assembler
or compiler changes are needed.

`CdReset` also matches with plain C under the existing stock GCC 2.8.1
profile. Removing its v1 pin alone, its read/write barrier alone, or both
preserves 100% upstream objdiff. The ordinary local `ret` now retains the
required move/branch sequence without source-level constraints.

## Psy-Q scalar CD setters

`func_8007C130` and `StSetMask` now use ordinary global assignments, with
stock GCC 2.8.1 and the existing per-file `-fcall-used-$1` option. Their
12-byte and 28-byte ranges match exactly. AT is allocated by the compiler;
there are no register variables, empty barriers, or instruction ASM. Without
that option, the sector setter uses v0 instead of AT; enabling it gives
100% upstream objdiff. No other compiler options are required.

The removed page types only encoded fixed addresses. The actual objects
are four-byte integers: D_8009B260 at 0x8009B260, g_CdStreamMask at
0x800C0DC0, D_800B6918 at 0x800B6918, and g_CdStreamEndSector at 0x800C0DBC.
These are declared separately; no large enclosing structure is inferred.
All AT uses in the two generated functions were checked against the retail
instructions, including the final store in each return delay slot.

## SPU register address shift

`_spu_FsetRXXa` now expresses `value >> shift` in C. One v0 pin for the
shift count and one empty read/write barrier on the existing a3 result pin
replace its old inline nop/srlv block. Removing the shift pin scores
99.51219%; removing the barrier scores 99.63415%, and reducing it to an
input-only barrier scores 95%. The existing offset, unit, shifted-result,
and final-address pins were tested individually and remain necessary.
The containing translation unit still has legacy division ASM and a stack
pointer constraint; this is not a clean-C match claim. A fully arithmetic
candidate and the remaining stock-MASPSX difference are recorded under
`proposals/spu_FsetRXXa`.

## TIM block accessors

All five functions in `psyq/libgpu/tim.c` now use the shared `TimFile`,
`TimBlock` and existing GPU `RECT` layouts. The file's first block begins at
byte 8; each block holds a four-byte length, eight-byte rectangle and pixel
words. With flag 8 set, that first block is the CLUT and its length locates
the image block. The loader no longer converts a block pointer through an
integer union. The two CLUT accessors and two image accessors return typed
rectangle/pixel pointers instead of untyped integer addresses.

The image accessors retain their two existing v1 length pins. Removing them
reduces each getter's upstream objdiff to 97.083336%; the tested stock CSE,
strength-reduction, force-memory and scheduler flags do not eliminate this
difference. No barriers or instruction ASM were added. With the pins retained,
all five functions score 100%. The three C callers of `Gpu_LoadTimImage`
now include the same pointer-based prototype, replacing their conflicting
integer argument/return declarations. Full retail main and overlay SHA checks
remain required for accepting the shared header change.

## GTE matrix control-register setters

`SetRotMatrix`, `SetLightMatrix`, and `SetColorMatrix` now use the shared
`gte_ctc2_*` hardware-transfer macros in `include/pe1/gte.h`. Rotation uses
control registers 0..4, light 8..12, and light-color 16..20. Every added macro
contains exactly one CTC2 transfer; the five ordinary memory loads remain
in each calling C function. No CPU instruction or scheduling operation was
moved into a macro.

The five t0..t4 pins in each setter were tested independently against its
complete generated text. None could be removed without changing the binary,
so all fifteen remain. This centralizes legitimate COP2 operations and does
not classify these functions as pure C or claim that hardware transfers have
been eliminated.

The existing `gte_mvmva` comment incorrectly described opcode 0x4A486012
as SF=0. Its bit 19 is set, so the comment now says SF=1 (12-bit fraction).
This follows the [PSX-SPX command encoding](https://psx-spx.consoledev.net/geometrytransformationenginegte/#gte-command-encoding-cop2-imm25-opcodes).
Only the description changed; the opcode and hazard slots are untouched.

## memmove control flow

The backward-copy arm now falls through the existing if/else join instead
of using a redundant goto/label. The complete object retains 100% upstream
objdiff. Tests removing the count pin, temporary-test pin, and return barrier,
individually and in combinations, did not match. The copy directions and
existing forward-path return value are unchanged.

## Matrix object boundary evidence

The local SDK archive `LIBGTE.LIB` extracted under `/tmp/psyq46b/LIB`
contains separate MTX_09, MTX_10 and MTX_11 objects exporting SetRotMatrix,
SetLightMatrix and SetColorMatrix. Each contains 48 text bytes, all identical
to its corresponding expected retail object, with no relocations in those
instruction sequences. This confirms the three object boundaries rather
than supporting consolidation into an invented common matrix TU.

The archive SHA-256 is `9f5b43f234c3037acf38180c8e8c85e778e442f3a216b87b07a8f99fbcffcdf3`.
The respective text SHA-256 values are:

- MTX_09: `095799477a23b46afa40d99dc027e7ef467cd14b8e135234d7e5b5c9ebc07883`
- MTX_10: `b9ce521ecd6cadeda48a0828b73f9b37d260c4f83e4d3b099334608f9e8fe598`
- MTX_11: `32169d0ce375b3a1a24a5beadfd11cd857ce0a89d610ddf3a1f3855fa6ed34f6`

`psyk list -c` exposes section, export and code records but no source filename
or language for these members. Neither the matching bytes nor the object
names prove original handwritten assembly. Their classification is unchanged;
no new exemption from decompilation is inferred from this evidence.

## DMA callback types

`setIntrDMA` and `startIntrDMA` share typed callback declarations through
`psyq_api_internal.h`. The setter accepts and returns a pointer to a
no-argument interrupt callback; initialization returns a pointer to that
setter, which `Sys_InitIntrManager` stores as such. This replaces the
inconsistent `void *` and integer return declarations. The handler table
uses ordinary callback-pointer indexing rather than a manual four-byte
shift and byte-pointer cast.

The setter accesses the dispatch register through a volatile local pointer.
The retail data word at
0x800956BC contains 0x1F8010F4, the DMA interrupt control register. This is
hardware qualification, not a scheduling-only volatile object. Initialization
retains its original unqualified single store: making that store volatile
prevents stock GCC 2.7.2 from placing it in the call delay slot and adds a nop.
Canonical
linker symbols identify that pointer and the handler table at 0x800956C0.

The table-base v1 pin was removable without changing the generated text or
relocations. Individual removal of the other setter pins did not preserve
the match; they remain, along with the existing empty barriers. The typed
setter and clear helper each retain 100% upstream objdiff. No CPU assembly
or tool modification was introduced.

## Interrupt callback dispatch layout and return values

`intr_callbacks.c` now dispatches through named, typed fields in
`InterruptDispatchTable`, replacing a homogeneous array of unprototyped
void functions. Offsets 4 and 8 hold DMA and interrupt setters; offsets
12, 16, and 24 hold reset, stop, and restart operations; offset 20 holds
the VSync setter. The first word remains explicitly unknown. Compile-time
assertions cover the setter offsets and this 28-byte layout.

The wrappers now pass their channel and callback arguments explicitly and
return the dispatched result. `ResetCallback`, `StopCallback`,
`RestartCallback`, `CheckCallback`, and `VSyncCallback` use the public
signatures from the local Psy-Q 4.6 `INCLUDE/LIBETC.H` (lines 65–71).
The VSync public API returns int, while its underlying setter returns the
previous callback pointer. Its single explicit pointer-to-int conversion
preserves that SDK interface on the target's 32-bit ABI.

`setIntrVSync` now returns the previous callback explicitly: the retail
function loads it into v0 before comparison and preserves v0 through both
paths to its return. The formerly void reconstruction only preserved that
value accidentally. `startIntrVSync` returns a typed setter pointer, and
`Sys_InitIntrManager` stores both setters through the shared layout.
The boot callback is now declared as a function rather than an opaque array.
No register pins, barriers, or instruction ASM are needed for these changes.

## Callback consumers and saved CD data handler

All C declarations of `InterruptCallback` and `DMACallback` now use the
shared pointer-returning prototypes in `psyq_callbacks.h`. The SPU, GPU,
CD and DS callers no longer declare those functions as returning void or
accepting callback addresses as integers. This follows the reconstructed
setter implementations; ignored return values remain ignored.

The local SDK `LIBCD.H` and `LIBDS.H` declare `CdDataCallback` and
`DsDataCallback` as returning the previous function pointer. Both wrappers
now express that return in C and share typed declarations with their callers.
`DsRead2` retains its saved DMA callback as a function pointer. Its existing
instruction ASM is unchanged and remains reconstruction debt.

`CdReadProgressState` offset 0x1C is now `DsCallback dataCallback`, replacing
an opaque integer token. `Save_ProcessDataCallback` passes that field to the
DS setter when the flags at offset 0x14 require restoration. Its original
pointer at offset 0x28 is converted back to the enclosing structure, so the
existing allocator barrier can remain while accesses use named fields.
This removes the negative word-index accesses; a static assertion verifies
the callback field offset. Other fields and their meanings are unchanged.

## DS event callback registry

The SDK's `DslCB` (`LIBDS.H`, line 98) takes an unsigned byte event and an
unsigned byte result pointer. `DsSyncCallback` and `DsReadyCallback`
(lines 236–237) exchange callbacks of that type. The reconstruction names
that signature `DsEventCallback` and uses it in both setters, their callers,
and the saved sync/ready fields of `DsAsyncReadState`.

The three-word registry at D_800B8AB0 now has named start, sync and ready
members. Only the latter two have recovered function-pointer signatures;
the first member retains its opaque integer representation. No callback
signature is inferred for the start slot. Its size assertion preserves the
existing 12-byte storage layout.

`CdRom_InitAsyncRead` no longer casts either registered function to int.
`DsRead2` keeps the previous sync callback as a typed function pointer.
`CdRom_ReadDoneCallback` restores the saved setters through the existing
async-state layout instead of raw negative indices. Its result parameter
and the unused parameters of `CdRom_BreakSyncCallback` now agree with the
registration ABI. No pins, barriers or instruction ASM were introduced.

## DsRead2 data callback registration

`DsRead2` now calls `DsDataCallback(data_ready_callback)` directly in C.
Stock GCC 2.8.1 emits the original LUI, JAL and low-address ADDIU in the
call delay slot, so the handwritten call window and its result-register
pin are removed. The saved data callback already has its recovered
function-pointer type.

The saved-position s2 pin was independently removable with identical full
text. Removing the saved-mode s0 pin, either alone or together with the
position pin, changed the binary while the saved local copies remained.
Using the original `mode` and `pos` parameters directly subsequently removed
the mode pin and both unnecessary local copies with identical full text.
There are now no register pins in this TU. No new empty barrier was
introduced. Full linked SHA remains the acceptance gate.

The preceding flag-setting assembly remains. A complete C trial, using an
ordinary branch and a local constant constrained only in the nonzero arm,
reproduces the flag window's instruction count and branch layout, but uses
ordinary temporary address registers instead of the original AT register.
Making AT allocatable also changes the argument and condition temporaries;
the tested constrained variants did not match. This partial removal does
not classify the containing TU as pure C.

## DS reset structure reuse

`CdRom_ResetDsReadSystem` uses the shared `CdQueuedCmdSlot` layout for
its three command slots: state words at offsets 0, 16 and 32, followed by
result-byte clears at offsets 4, 20 and 36. The source now names the state
and result members instead of using integer indices and byte casts.
The six-word local queue placeholder is replaced by `CdDsReadQueueEntry`,
and the four-word callback-slot placeholder by `DsReadCallbackSlot`.
The canonical queue symbol is verified at 0x800A3540.

The callback-slot clear loop retains its existing explicit byte offset.
Replacing it with an indexed record access introduced repeated index shifts
and changed the retail instruction sequence. The final cursor write also
retains its legacy AT/page constraint: an ordinary scalar-symbol store
with this TU's no-split-addresses profile moved out of the following call's
delay slot and added a nop. Split-address profiles changed other global
accesses. Neither trial was accepted as a matching cleanup.
No new constraints, instruction assembly or aliases were introduced.

## SPU common-volume constraints

`SPU_WriteVoiceRegs` retains its two value constraints but no longer marks
all memory clobbered at either boundary. Removing the initial memory
clobber and the final memory clobber independently preserved the complete
text; the combined removal is checked with the full retail executable.
The initial read/write operand still separates the initial left value, and
the final input still preserves the right temporary and common return path.
Removing that final operand or the whole final barrier did not match.
Making the initial operand input-only or removing its barrier also failed
full-text comparison after the combined memory-clobber removal.

Removing the right-value t0 pin rotates the mask, all-fields predicate and
right temporary among t0..t2. Reordering its initialization or widening the
value did not recover the target allocation. The pin remains. These changes
narrow scheduling constraints; they do not classify this TU as pure C.

## Memory-card interrupt register view

The retail pointer at 0x8009B784 (`g_MemCardState`) contains 0x1F801070.
It addresses I_STAT at offset 0 and I_MASK at offset 4, as documented in
[PSX-SPX's interrupt register map](https://psx-spx.consoledev.net/interrupts/).
The shared type is now `MemCardInterruptRegisters`, with named volatile
32-bit status and mask fields. `MemCard_TimerReadyCallback` checks those
fields; `MemCard_WaitReadyForTransfer` acknowledges the controller/card IRQ
through status. Their access widths and instruction sequences are unchanged.

The earlier header comment incorrectly described the RAM words at
0x800A5AC0/4 as another instance of the same state type. Those words are
separate signed counters: the timer callback compares them with 150 and
increments them, while `mem_card8` tests and resets them. No enclosing RAM
object is inferred here. The duplicate local interrupt-register typedef
in the transfer TU and two unused include_asm includes in the timer TU
were removed. No pin, barrier or instruction assembly was added.

A direct C reconstruction of the timer's initial flag/store and increment
window remains unmatched: stock GCC 2.7.2 moves the flag store out of the
branch delay slot and changes the timer address/value registers. GCC 2.8.1
profiles also changed other functions in that TU. Its legacy instruction
ASM and the callback-pending page constraint remain pending reconstruction.

## Shared memory-card SIO register layout

The retail pointer at 0x8009B788 contains 0x1F801040, the controller/card
SIO0 block. `MemCardSioRegisters` now gives both timer and transfer TUs one
shared volatile MMIO view: data at 0, low status at 4, mode at 8, control
at 10 and baud at 14. Status is exposed as two halfwords so existing
halfword accesses retain their widths. Layout assertions verify the used
offsets and 16-byte span. Register names and addresses follow
[PSX-SPX's SIO map](https://psx-spx.consoledev.net/serialinterfacessio/).

This removes the unused, incomplete TimerRegs view, the duplicate anonymous
SIO layout and the raw halfword-array declaration of the same pointer.
The timer's `+0xE` store is now `baud = 0x88`; the poll loop names the status
field instead of indexing halfword 2. Pointer and register volatility are
shared consistently. Full main and overlay SHA checks cover the changes.

The explicit nop before MemCard_WaitStatusBit2's loop remains. Removing it
scores 99.5% against the matching full TU: the loop target includes a
load-delay nop instead of beginning on the status load. Stock `-mdebugf`,
either scheduler-disable option, and empty memory/input/tied-pointer
barriers did not recover the target. No added constraints are retained.

## Memory-card wait-loop control flow

`MemCard_WaitReadyForTransfer` now uses a while loop instead of its two goto
edges and loop/ready labels. The loop keeps reloading the volatile SIO
pointer, preserves timeout failure, and acknowledges control bit 4 only
when ready. Both functions in the transfer TU retain 100% object comparison
against the previously SHA-verified build before full-image verification.

The status-bit-2 nop blocker is now traced to pinned MASPSX's
`_handle_nop_before_next_instruction`: when the next item is a label, it
emits that label before the required load-delay nop and skips the original
label occurrence. The compiler's raw output has the loop label immediately
after the SIO pointer load. Thus the remaining difference is the target of
the backward branch, not an extra or missing instruction. GCC 2.8.1 with
and without address splitting did not resolve it, and also changed the
other function. The tool is unchanged; no label or binary postpass is used.

## SPU transfer callback types (2026-09-09)

The object at `0x8009B434` is consistently declared as
`SpuCallback volatile _spu_transferCallback` in `psyq_spu_internal.h`.
The DMA interrupt handler and `SPU_StartDmaRead` already required volatile
accesses; the latter now saves a function pointer, rather than a signed
integer. The upload/read wrappers, callback setter and shutdown use this
same declaration, removing the incompatible integer views.

`SpuSetTransferCallback` returns the previous callback. Its shared prototype
now also covers the AKAO transfer helpers, which previously declared a void
return. This agrees with Psy-Q 4.6 `LIBSPU.H`'s
`SpuTransferCallbackProc SpuSetTransferCallback(SpuTransferCallbackProc)`.
All changes preserve the complete retail executable.

Separating the reverb-area pointer from the remaining byte count in
`SPU_StartDmaRead` was also tested. A typed `base + mode` expression retains
the instructions but swaps the live mode and area registers (`s0`/`s1`),
including their save order. This independent cleanup is not accepted.

## SPU IRQ callback registration (2026-09-09)

The IRQ setter (`Spu_SetTransferMode`, a historical project name), its AKAO
initialization caller, and the IRQ/DMA registration wrappers now share
`SpuCallback` and their declarations through `psyq_spu_internal.h`. The IRQ
setter reads `_spu_IRQCallback`, the existing symbol at `0x8009B438`; shutdown
no longer declares that same callback storage as an integer.

The setter's legacy page-address store remains. Replacing it with an ordinary
symbolic assignment under its unsplit profile adds a nop after the call.
Stock GCC 2.8.1 with address splitting and `-fcall-used-$1` fills the call slot
but also shares the high address across the preceding load, changing the
prologue and removing a required `lui`. Local previous-value pins, scratch
clobbers and scheduler variants did not restore the complete sequence.
A direct C shutdown candidate also remains nonmatching; its legacy
instruction blocks have not been promoted or hidden.

## SPU read/write wrapper return constraints (2026-09-09)

`Spu_ReadFromSpu` and `Spu_UploadToSpu` now return their capped transfer size
directly. Both local `$2` result pins are unnecessary with the current shared
volatile callback declaration and stock GCC 2.8.1 unsplit profile. Removing
each pin gives 100% object agreement against the SHA-verified build; both
wrappers now contain no register pins, barriers or instruction assembly.
The complete main executable and all 191 overlays retain their retail SHA-1.

The internal `_spu_Fr`/`_spu_Fw` declarations now live beside the SPU callback
API. In particular, `_spu_Fr` is declared with its implemented signed-size
return and argument types, replacing the reader's incorrect void-return
declaration. The wrapper's capped unsigned size is representable as `s32`.

## SPU DMA command control flow (2026-09-09)

`_spu_t` no longer requires the empty read/write barrier on `control`.
For DMA command 3, expressing the final CHCR selection as
`direction != 1 ? control | 0x201 : 0x1000200` keeps low-bit preparation
in the relevant branch. Stock GCC 2.8.1 then places the `ori` in the branch
delay slot naturally. The accepted source uses the equivalent `if/else`.
Simply deleting the barrier while retaining the unconditional OR schedules
that OR before the DMA register writes and leaves a nop in the branch slot.

All four functions in `psyq/libspu/spu_transfer.c` compare at 100% against
the prior SHA-verified object. The entire TU now contains no pins, barriers
or instruction assembly. Full main and all 191 overlay SHA checks pass.

## SPU register read width (2026-09-09)

`_spu_FgetRXXa` now reads a register once into a `u16` local through a volatile
halfword pointer and reuses that value for both return paths. This replaces
three repeated byte-address/dereference expressions in the source, which the
old compiler had coalesced into one machine read. Typed register indexing
replaces byte-pointer arithmetic. An explicit `u32` conversion before the
left shift preserves unsigned arithmetic.

The halfword local is significant: a `u32` local changes register allocation;
`u16` produces the target naturally, without a pin or barrier. All seven
functions in the production GCC 2.7.2 TU compare at 100%, and the full main
and 191 overlay checks pass. The separate GCC 2.8.1 proposal still needs its
getter constraint; applying this source change there does not match.
The debt scanner classifies the new scalar `(u32)value` conversion as a
`pointer_integer_casts` occurrence; no pointer-to-integer conversion was added.

## SPU register access API and reverb volume fields (2026-09-09)

`_spu_FsetRXX` now indexes halfword registers instead of repeating byte-address
arithmetic. Its existing access qualification is preserved: making the store
volatile changes scheduling and is not part of this matching cleanup.
The three generic register helper prototypes are shared in the SPU internal
header, including the unsigned value and return types of `_spu_FsetRXXa`.

`SPU_StepDmaRead` now uses the shared `SpuRegs` view instead of declaring the
same `_spu_RXX` object as a volatile-halfword pointer. Five literal register
accesses become `spucnt`, `reverb_volume_left` and `reverb_volume_right`.
The latter fields at offsets `0x184`/`0x186` were incorrectly named master
volume; [the SPU register map](https://psx-spx.consoledev.net/soundprocessingunitspu/#reverb-volume-and-address-registers-rw)
identifies them as reverb output volume at `0x1F801D84`/`0x1F801D86`.
The layout and all volatile access widths remain unchanged.
Full main and all 191 overlay SHA checks pass.

## Shared SPU common and reverb register window (2026-09-09)

`SpuRegs` now covers the 0x200-byte window through the reverb parameter
registers. It names the main-volume and CD/external input-volume pairs and
the 32 halfword reverb registers. A compile-time size check protects the
window layout. The register meanings and addresses follow the
[SPU register map](https://psx-spx.consoledev.net/soundprocessingunitspu/).

`SPU_ReadRegister`, `SPU_WriteVoiceRegs`, `_spu_setReverbAttr` and
`Akao_SetMasterVolume` use this shared view instead of incompatible pointer
declarations. The reverb parameter setter replaces all 32 literal byte
addresses with indexed halfword fields; the original mask tests and write
order remain intact. All four functions match their prior objects at 100%,
and the full main and all 191 overlay SHA checks pass.

`Spu_SetGlobalVolumeField1AA` was tried with the shared volatile control
field but remains unchanged: the qualified store leaves the return delay
slot and adds a nop. The existing constraint in `Akao_SetMasterVolume` also
remains necessary; replacing it with an ordinary pointer assignment changes
code generation. No new pins or barriers are added.

## SPU per-voice register structure (2026-09-09)

`SpuVoiceRegs` describes the eight halfword registers of one voice, with a
compile-time 16-byte size check. `SpuRegs.voice[24]` replaces the opaque
0x180-byte prefix without moving the common register fields. The layout
follows the [SPU voice register map](https://psx-spx.consoledev.net/soundprocessingunitspu/).

`SpuGetVoiceEnvelope` now reads the named volatile envelope field using a
voice index, removing the integer representation of the pointer and the
literal envelope offset. Writing the pointer sum with the index first
preserves the target register allocation. `Spu_SetVoiceAttr` uses the same
register type for its left/right stores, retaining its halfword-index
intermediate; a direct voice index collapses two shifts and does not match.
Both functions retain 100% object agreement and the full main and all 191
overlay SHA checks pass. No pins or barriers were introduced.

## Identified Psy-Q noise-clock setter (2026-09-09)

The function at `0x80089EB8`, formerly `Spu_SetGlobalVolumeField1AA`, is
`SpuSetNoiseClock`. Psy-Q 4.6 `LIBSPU.H` declares
`long SpuSetNoiseClock(long n_clock)`, and the recovered Psy-Q `s_snc.c`
implementation clamps the same six-bit value and writes SPUCNT bits 13:8.
The production function now has this SDK name/signature and lives under
`psyq/libspu`; its symbol and source manifest were updated together.
The neighboring eight-byte padding subsegment remains separate.

The local partial register type is now named `SpuNoiseControlRegs` and its
control field `spucnt`, replacing misleading AKAO state/anonymous-field names.
This is still a local unqualified register view: switching to shared volatile
`SpuRegs` moves the store out of the return delay slot and adds a nop under
both stock compilers. Removing either or both existing pins also changes the
function. These unresolved constraints remain visible; this rename is not a
claim of a newly pure C match. Full main and 191 overlay SHA checks pass.

## DMA callback setter constraints (2026-09-09)

`setIntrDMA` no longer has a channel pin, channel input barrier or an
uninitialized callback variable pinned to `$zero`. The stock per-TU option
`-fno-cse-follow-jumps` preserves a literal-zero callback-table store instead
of substituting the callback argument known to be zero on that branch.
This permits `*slot = 0` and direct use of the `channel` parameter.

Both functions in `libapi/intr_dma.c` compare at 100% against the prior
SHA-verified object. Each remaining pin and barrier was tested for removal
after the final source shape; each still changes the output. Full main,
191 overlay SHA checks and source/debt gates pass. The stock compiler and
MASPSX are unchanged.

## DMA channel-mask expressions (2026-09-09)

The disable branch of `setIntrDMA` now computes its channel mask directly
as `~(1U << (channel + 0x10))`. This removes the `bitClear` temporary and
its register pin. With this expression shape, the enable branch's `bitSet`
pin is also unnecessary. The unused `mask` local was removed.

Both TU functions retain 100% object agreement. Removal trials for each of
the remaining four pins and two memory barriers still change the output.
Collapsing the enable branch to a similar combined expression is nonmatching
(99.023254%), so its current ordinary temporaries remain. Full main and all
191 overlay SHA checks and source/debt gates pass. No toolchain change was
needed beyond the already selected stock profile.

## DMA setter barrier scope (2026-09-09)

The two exit constraints in `setIntrDMA` now name only `*dmaReg` as a memory
input instead of clobbering all memory. Each follows the volatile DMA control
store and preserves that store's order before the return sequence. Empty
assembly emits no additional register read or instruction. Both functions
retain 100% object agreement; full main, 191 overlay SHA checks and source
gates pass. These two barriers remain visible in the debt inventory.

A shared pinned return variable and using the callback parameter directly
were also tried; both change the register/return sequence. For `v_wait`,
using the actual first parameter and symbolic timeout text yields a 95.76316%
pure-C candidate under GCC 2.7.2: the address expansion leaves an extra call
delay-slot nop. A local replacement for its global register variable still
changes ADDIU to ORI. No production change was accepted for those trials.

## DS asynchronous read callback type (2026-09-09)

`DsAsyncReadState.callback` is now `DsAsyncReadCallback`, a function pointer
taking an integer status and two pointers. `CdRom_AsyncCallback`'s retail
indirect calls pass status in `a0`, its data pointer in `a1` and a local
record address in `a2`; the reconstructed read-done path uses the same ABI
with a null third argument. This field is no longer stored as an integer.

`CdRom_InitAsyncRead` and both C callers share one declaration, including
the actual integer return type. `CdRom_ReadDoneCallback` reads the named
callback field instead of casting a negative-index integer slot. The
ASM-backed progress callback is declared with its actual callback ABI;
`GD_disk_kind` accepts those arguments and explicitly retains the original
low-byte conversion of the event. Full main and all 191 overlay SHA checks
pass, without new pins or barriers.

Removing each existing DS read-control barrier was also tested: the ready
helper drops to 70.75%, initialization to 98.181816%, and break to 99.8%.
They remain in production pending scheduling/source reconstruction.

## DS queued command parameter pointer (2026-09-09)

`CdDsReadQueueEntry.parameter` replaces integer `arg0C`. The retail queue
producer saves its second argument at offset 0x0C after optionally copying
four bytes from it; `CdRom_PollPendingDsRead` passes that field to the command
issuer. These producer and consumer accesses establish the pointer type.
The queue reset uses the same named pointer field.

All C callers of `Render_AllocParticleNode` now share a prototype with a
pointer second argument; the existing historical function name remains.
`DsControlF` uses the Psy-Q LIBDS.H signature with byte command and byte
pointer parameters. Break/cancel callers use typed null pointer temporaries.
The two later queue fields remain unnamed integers pending evidence of their
meaning. Full main, all 191 overlay SHA checks and source/debt gates pass.

The `DsReadBreak` argument barrier remains necessary: its removal substitutes
the known-zero `a1` value for `$zero` when preparing `a2` (99.8%); removing
just the command operand also changes code generation (92%).

## DS queue storage window (2026-09-09)

`CdDsReadQueueWindow` describes the observed contiguous memory window: eight
24-byte queue entries at 0x800A3540, queue state at 0x800A3600, read index at
0x800A3604, and pending count at 0x800A3608. A compile-time assertion fixes
the pending-count offset at 0xC8. This is a shared memory view, not evidence
that the original source declared the entire region as one object.

`Spu_GetQueueEntryPtr` now reads the named queue-state field and derives its
entry base through that window; `CdRom_PollPendingDsRead` uses the same base
calculation. The helper uses the shared pending-count symbol instead of a
second C extern alias. The entry's `arg10` and `arg14` remain unidentified.

The helper's final pointer/integer addition and existing constraints remain:
a direct pointer addition exchanges the two ADDU source registers. The named
window preserves the retail main SHA without new pins or barriers.
Full main and all 191 overlay SHA checks and source/debt gates pass.

## Original QSORT translation unit (2026-09-09)

Psy-Q 4.6 `LIBC2.LIB` contains one 400-byte QSORT object: `qsort` at offset
0, its byte-swap helper at offset 0x150, and a final padding word. Comparison
with retail 0x800723A4..0x80072534 finds 93 identical words and seven differing
words, exactly at the SDK relocation offsets 0x68, 0x70, 0x90, 0xCC, 0xF0,
0x104 and 0x11C. Four calls address the helper within this same object. This
is direct object evidence for merging the two former C subsegments and their
trailing padding, rather than a speculative organization change.

`psyq/libc/qsort.c` now contains the complete TU. The helper definition shares
its caller's `void *` parameter types and converts them to byte pointers
locally. The separate `util/Mem_SwapBuffers.c` and padding ASM subsegment are
removed. Both functions compile with stock GCC 2.8.1. One new helper pin on
`left` (t0) preserves the order of its two initial pointer moves; omitting it
swaps those instructions (98.666664%). The existing helper barrier still
cannot be removed (98%). Each of the three pins and two barriers in the
merged TU was tested separately; none is currently removable. The debt
baseline explicitly records the additional pin rather than hiding it.

Both functions match their previous SHA-verified instruction streams at
100%, and the complete main executable retains its retail SHA-1.
All 191 overlay SHA checks and source, organization and debt gates also pass.

## LIBCD callback pointer types (2026-09-09)

The shared header now defines the SDK `CdlCB` type, `void (*)(u_char, u_char *)`,
and uses it for the sync, ready and read fields of `CdCallbackDataPage`, the
sync field of `CdCallbackDataWindow`, and the corresponding globals.
`CdReadCallback`, `CdSyncCallback`, `CdReadyCallback` and the internal read
setter now accept and return function pointers. The command retry wrapper
saves/restores a typed callback. Reset paths and C callers share these
prototypes instead of local integer or void-return declarations.

Psy-Q 4.6 LIBCD.H declares all three public setters with this signature.
The retail interrupt dispatcher independently loads a byte event into a0
and a result-buffer pointer into a1 before each sync/ready callback call.
`CdRom_InitDsCallbacks` now declares its two handlers with their actual C
signatures and registers them using explicit CdlCB conversions, replacing
integer address casts. Their existing int event parameters remain: changing
them to u_char alters the retail masking instructions (91.02128% and
93.45946%). The conversions document this remaining PSX ABI boundary; they
are not a claim of full ISO C function-type consistency at those two handlers.

The main executable remains byte-identical, without new pins or barriers.
All 191 overlay SHA checks and source, organization and debt gates pass.


## LIBDS disk-type query

`DsGetDiskType` is C compiled with stock GCC 2.8.1 and unsplit addresses.
It retains one empty v1 clobber on the early return of 16: removing it changes
`addiu v0,zero,16` to a copy of the comparison constant from v1 (98.98305%
function match). No instruction ASM or register pin remains. Its polled
`g_DsDiskType` state is shared as a volatile int with the disk-type callbacks.

The adjacent `dstype.c` callbacks still require GCC 2.7.2. A trial compiling
all three functions together under GCC 2.8.1 moved both callback stack restores
into the return delay slots, unlike retail. Keep that compiler boundary until
all three bodies can match as one TU; the query is not claimed as a restored
complete original translation unit.


## LIBCD controller flush

`CD_flush` is entirely C. The controller acknowledgment loop and event reset
use volatile byte accesses; `CdInterruptEvents` names the adjacent sync,
ready and data-end event bytes at 0x8009B294. `getintr` writes event 4 to the
third byte and copies it to the ready byte on data end. The flush reads the
index-register pointer before writing the sync event, retaining the observed
ordering even if a byte store could alias the pointer's storage.

One v1 pointer pin and one tied empty pointer barrier retain the event-block
address in a register. Removing the pin gives 98.962265%; removing the
barrier, with or without the pin, gives 84.49056%. No instruction ASM remains.

## LIBCD second sector-transfer wait

The DMA-busy wait in `CD_getsector2` is C: it reads control bit 24 once,
then retains a local pointer and mask for repeated volatile reads when busy.
This replaces the former 14-instruction assembly block without adding pins
or empty barriers. A single `while (*control & mask)` produces a different
register allocation; the initial test and inner loop retain retail bytes.
Each of the function's three existing pins was also tested individually;
removing any changes the generated text.

The earlier CD-ready byte wait remains legacy ASM. The all-C trial matches
the remaining executable instructions but sends its back edge to the load
hazard nop instead of the byte load after it. The pinned upstream MASPSX
`_handle_nop_before_next_instruction` moves an immediately following label
before its inserted nop. Simple while, do/while, explicit first read, goto,
and an empty loop-body constraint did not avoid that difference. This is
not a reason to patch the assembler, add a scheduling instruction, or credit
the whole function as semantic C before the remaining wait is reconstructed.

## DS command event callback

`CdRom_CmdEventCallback` is entirely C and matches all 236 retail bytes.
Its second argument is the result-buffer pointer forwarded unchanged in a1
to the user callback. The caller and definition now share this signature.
The sync/ready callback globals use volatile function-pointer storage, matching
the callback reload after checking the system's enabled flag.

The command-state view replaces the anonymous ready window; its enclosing
system state is recovered using the asserted command-member offset. One a2
pointer pin and two tied empty pointer barriers remain. With the final source,
removing the pin gives 98.47458%, the command pointer barrier 87%, and the ready
pointer barrier 86.33898%. The previous a3 event pin was removable. No
instruction ASM or compiler/assembler modification is used.

## DS ready event dispatch

`CdRom_ReadyEventDispatch` is entirely C under stock GCC 2.8.1 with
`-mno-split-addresses -fno-schedule-insns`. These options preserve the saved
argument setup and independent symbolic callback loads. The callback uses
the shared volatile `DsEventCallback` pointer and its byte-event signature.
An a0 pin and an input-only empty barrier place the already masked event
before the final callback load. Removing either gives 96.75676%; the old
command-state pointer barrier is unnecessary and was removed.

The state accesses now use `CdRomCommandState` and its asserted offset within
`CdRomSystemState`, replacing the anonymous ready-window alias and negative
word index. All 148 bytes match, and the main executable plus all 191 overlays
retain their retail SHA-1. No instruction ASM or toolchain patch remains.
