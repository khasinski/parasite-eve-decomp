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
