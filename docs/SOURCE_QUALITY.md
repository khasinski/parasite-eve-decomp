# Source quality levels

Progress is reported in code bytes first. Function count is useful for work
planning, but thousands of short wrappers and overlay handlers make it a poor
headline measure.

The progress report deliberately credits only `semantic_c` units from modules
whose full linked binary matches retail. It does not credit generated or
original assembly, BIOS trampolines, units containing ordinary inline CPU
instruction implementations, or data emitted into `.text`. GTE/COP2 macros
allowed by `ASM_AND_GTE_POLICY.md` remain semantic C. Pins and empty compiler
barriers are eligible matching constraints, but their separate debt counters
prevent the byte-match number from presenting them as clean C.

Function boundaries are inferred during retail disassembly, so their global
count is necessarily provisional. The target generator also borrows symbol
names and sizes from verified C objects to make objdiff pairing practical.
Consequently the function metric is not independent of the source split.
`make progress-audit` checks target/base `(offset, size)` consistency and
prevents mismatching or non-semantic units from receiving credit, but the
code-byte metric and full linked SHA-1 remain stronger evidence.
After objdiff runs, `make report-audit` independently verifies that the
published matched-code and matched-function totals contain only `semantic_c`.

### Main GP data classification

The retail range at file offsets `0x818A0..0xB24A0` (199680 bytes) is data,
including handler pointers and map tables, not a large function. The old
`main/dtail_gp` ASM split placed it in `.text`. Relabeling its symbols as
objects did not fix section-based reporting: the report still counted
199680 code bytes and 317 apparent functions, all unmatched. That inflated
the remaining-work denominator, not the semantic-C numerator.

The split now emits this range as `.data`; a separate field-engine segment
preserves the following code's retail address `0x800C1CA0`. The full main
executable still passes its unchanged SHA-1 check. Generated data units are
explicitly tagged in the report config, and the report audit rejects any
code bytes or function counts attributed to them. This is a metric repair,
not newly decompiled code or reconstructed C data structures.

### SPU transfer reconstruction

`Spu_SetTransferMode` at `0x80085D84` is actually a callback setter: it
replaces `D_8009B438`, calls `_SpuCallback` only when the callback changes,
and returns the previous callback. Its 60-byte C implementation uses stock
GCC 2.8.1 with `-mno-split-addresses`. One `$at` pointer pin and one empty
barrier preserve the global store in the call delay slot; no instruction
ASM is used. The explicit page and index resolve to `0x8009B438`.

The symbolic-only candidate scored 160 with decomp.me's asm-differ settings;
removing the barrier scored 305 and removing the pin scored 20 against the
relocatable target. The retained candidate scores 10 in that comparison
solely because its store address is absolute rather than relocated. After
linking, the same decomp.me comparison against retail scores zero, and the
full main executable passes its unchanged SHA-1. The pin and barrier are
recorded in the debt baseline, not presented as unconstrained C.

The adjacent `_spu_Fw` (132 bytes) and `_spu_Fr_` (168 bytes) use the same
stock compiler profile but need no pins, barriers or instruction ASM.
`_spu_Fw` selects DMA or programmed-I/O transfer and returns the requested
size. `_spu_Fr_` programs the SPU transfer address and DMA registers for a
read. Both score zero against independent retail objects as well as after
linking; the latter uses the existing named DMA globals rather than new
address aliases. The pointer-to-integer conversion writes the DMA address
register and is explicitly included in the debt baseline.

The 640-byte `_spu_t` dispatcher completes the former `psyq/spu6` unit.
It handles address setup, read/write mode selection with bounded polling,
and DMA launch with the block count rounded up to 64-byte blocks. Its
argument cursor follows the stock PSX compiler's varargs stack layout;
this is ABI-specific C, not a portable host implementation. The DMA address
is reloaded through its volatile global, preserving the retail accesses.

It needs no register pins or instruction ASM. One empty volatile barrier
keeps CHCR bit preparation after the DMA register writes. A provisional
`$a2` pin was removed without affecting the match. Removing the retained
barrier gives decomp.me score 160; keeping it gives zero against the
independent retail object. The full executable SHA-1 remains unchanged.

`_spu_FiDMA` (188 bytes) finishes a DMA transfer, polls the cleared transfer
mode with a timeout, and invokes the transfer callback or delivers the BIOS
event when no callback is installed. Its stock GCC 2.7.2 implementation
retains both retail reads of the callback through a volatile declaration.
One empty `$v1` clobber marks the poll counter dead before notification;
without it the delay-slot pass emits an extra counter correction on loop
exit. There are no register pins or instruction ASM. The full executable
continues to pass its original SHA-1 check.

`Spu_SetVoiceAttr` (364 bytes) sets the left and right volume registers and
their sweep modes for one voice, then runs a two-iteration delay. Both switch
tables are emitted by stock GCC 2.8.1; the four bytes after the second table
remain a separate retail data split, not padding invented in C. The retained
implementation uses an ordinary local work structure, no register pins, and
one empty memory barrier before initializing the delay counter.

Removing that barrier allows the counter store to move before the seed store
and leaves the jump delay slot empty. With both objects linked under the same
section layout, decomp.me's asm-differ score is 165 without the barrier and
zero with it (9100 maximum). Raw relocatable comparison also displays the C
object's jump-table reference annotations, so it is not the zero-score check
used here. The normal full executable passes its unchanged retail SHA-1;
objdiff credits all 364 code bytes and one new semantic-C function.

`SpuSetCommonAttr` (892 bytes) uses its original Psy-Q API name. Its
reconstructed 40-byte `SpuCommonSettings` input contains
the update mask, master volume and sweep modes, CD and external-input volume,
and their reverb/mixing switches. A zero mask selects all settings. The layout
test checks every field offset and the total size; the existing sequencer
caller now passes its settings pointer through the correct prototype.

The stock GCC 2.8.1 implementation retains one `$t0` pin for the right-volume
temporary and two empty barriers. The initial barrier prevents copying the
left zero into the right temporary; the final barrier preserves the temporary
and shared return block. Removing the pin scores 160, removing the initial
barrier scores 55, and removing the final barrier scores 745. The retained
candidate scores zero out of 22300 after independently generated retail and
C objects are linked with the same layout. A provisional second pin was
removed without changing instructions. The normal full executable also
passes its original SHA-1; the switch tables are compiler-generated rodata,
not ASM or data counted as code.

`SpuSetReverb` (204 bytes) uses its original Psy-Q API name. Zero disables reverb; one enables it
unless the work area conflicts with the SPU allocator and the override is
not one. Other arguments leave the state unchanged. It returns the resulting
enable state. Its stock GCC 2.8.1 implementation needs no pins, barriers, or
instruction ASM, scores zero against the independent retail object, and
passes the full executable SHA-1. The caller now has the correct return type.

The matching source now also uses the original internal Psy-Q symbols
`_spu_rev_flag`, `_spu_rev_reserve_wa`, and `_spu_rev_offsetaddr` at
`0x8009B390`, `0x8009B394`, and `0x8009B398`.

`SPU_StepDmaRead` (460 bytes) selects a reverb preset, rather than reading
DMA data. It validates the preset and work-area allocation, copies a 68-byte
register preset, configures delay/feedback defaults, temporarily disables
reverb, clears effect depth, writes the register set and work-area address,
and restores the previous enable bit. Flag `0x100` requests a work-area clear.
The shared `SpuReverbRegisterAttrs` layout is checked by the header test.

One index pin remains with stock GCC 2.8.1; two provisional pins and an empty
barrier were removed. The retained function scores zero out of 11500 after
binding relocations to the same addresses. Removing the remaining pin scores
30. The raw object comparison has a separate 30-point symbol-expression
difference: retail refers to `0x8009B3AC`, while C addresses element one of the
delay/feedback pair at `0x8009B3A8`. They resolve to the same address, and the
full executable SHA-1 and all 460 bytes in objdiff match. Preset, work-area,
mode and depth globals now use reverb-specific names, including the depth
alias formerly described as master volume.

Source classification is a conservative source-text heuristic, not a full
preprocessor or proof of semantic reconstruction. It joins adjacent ASM string
literals and rejects nonempty unrecognized templates, but does not expand all
headers/macros. Automatic COP2 exemptions now require fully visible transfer
instructions and `nop` slots only. Mixed CPU/COP2 expressions and opaque
template fragments are constrained, pending surgical source reconstruction or
separately documented review. This does not certify unexpanded header calls.
The retrospective also found ordinary CPU packing, shifts and stores in seven
GTE-named helpers. Their callers, including direct `.inc` template users, are
explicitly quarantined by the classifier. This list is a stopgap for known
violations, not a complete header-expansion audit. LoadAverageByte and
LoadAverageCol now use only MAC transfers in the replacement helpers; their
shifts and byte stores are C. The remaining 29 affected functions lose semantic
credit until corrected. The two repaired functions were previously counted,
so their repair is not two newly added functions.
RotTransPers3 (84 B) and RotTransPers4 (120 B) subsequently replaced their
CPU flag-store helpers with C stores. Each needs one empty memory barrier
after the store to keep the return delay slot occupied by the depth shift.
This restores two quarantined functions, leaving 27 of that audited group.
The retired helper names remain quarantined to prevent accidental reuse.
LoadAverageShort0 and LoadAverageShort12 (136 B each) then moved vector
unpacking, packing and stores into C, leaving 25 functions in that group.
Local volatile accesses preserve the original word-load width and store
order; these are matching constraints, not claims that the buffers are MMIO.
The existing output pin moved from the deleted helper into each caller.
No empty barriers were needed after minimization.

The vector packing window was subsequently replaced with matching C in all
25 remaining users, using `gte_ldv0_word3` and `gte_ldv0_word3_at`. A second
review then found CPU stores in `gte_stir123_matrix_column` and, in the
renderer, `gte_stsz3_s16`. These stores are now C too; the replacement helpers
only transfer COP2 state and preserve required hardware delay spacing. Their
retired names stay quarantined. This repairs withdrawn credit; it does not
represent 25 newly discovered functions.

Tightening the visible COP2 exception flagged another 362 configured units,
mostly repeated room handlers, for review. The 119 `RoomLib_HandlerD` users
now calculate their MAC output addresses in C instead of ASM `addiu` windows.
The 119 HandlerB and 119 HandlerC users, together with five scene-reset B
variants, subsequently moved their address windows into C too. All 243 linked
function ranges match retail. No new pins remain after minimization; the
included implementations retain two new barriers per B variant and one per
C variant. These are eligibility repairs, not newly discovered functions or
changes to retail targets. Closing this known group does not certify every
unexpanded header against the policy.

A subsequent header inventory found ordinary CPU instructions in
`ROOMLIB_LOAD_S16`, `ROOMLIB_LOAD_PTR`, `ROOMLIB_LOAD_U16` and
`ROOMLIB_DIV_V0_A0_CHECKED` in `room_lib.h`. Known calls are now quarantined:
10 signed-halfword users (5,760 B), one pointer-load user (676 B), and
228 checked-division users (208,848 B). The unsigned-halfword helper had no
source users in the inspected tree. The division macro's raw `.word` sequence
implements ordinary CPU division and exception checks, not GTE operations;
it cannot receive semantic-C credit merely because it reproduces a compiler
expansion. These 239 withdrawals are independent of the 243 B/C repairs.

The replacement sources now express those divisions and loads in C. The arc
callers select stock MASPSX `--expand-div`; no custom instruction expansion is
used. The pointer-load caller also contained `ROOM_M089_LOAD_EFFECT_LOOKUP`,
an ordinary CPU table lookup now written in C. All five retired helper
definitions have been deleted and their names remain quarantined against reuse.
Restoring these 239 functions requires full linked retail verification, not
merely removing their helper names from the sources. This is recovered semantic
credit, not 239 newly discovered algorithms. Two empty barriers remain in the
spawner; 456 arc pin instances and three spawner pins were removed after exact
comparison. The source-only ledger does not expand those template pins.
All 191 overlays were subsequently rebuilt and passed retail SHA-1 checks;
the main executable also passed `make check`. The source-only suite has 83
passing tests, including guards against reintroducing the deleted helpers.

The source-text debt counters do not expand all shared headers or `.inc`
templates. In particular the new word-stride helper has two pins and two empty
barriers per expansion, and the draw template has an additional input barrier.
Those constraints are real even where the current ledger does not count them.
A preprocessed debt inventory is still required; unchanged counters do not
prove a change is crutch-free.

The 2026-09-05 classifier correction withdrew semantic credit from four existing
CPU-ASM implementations (1,308 bytes): Task_SetEntityActionAndWait,
Task_TurnTowardPointStep, Entity_GetDistanceComponents and Entity_AllocBlock.
The corresponding asm-constrained debt baseline increase records previously
missed debt, not permission to introduce more instruction ASM.

Those four implementations were subsequently restored to semantic C by replacing
their CPU instructions with C address calculations and loads. Entity_AllocBlock
uses a pointer register pin; the other three use one extra declaration bound to
the same linker symbol to prevent old GCC from sharing an address calculation.
These aliases are matching constraints, not evidence of separate globals or
reconstructed structures. Two existing pins were removed in the process; no
new barriers or toolchain changes were needed. Full main SHA verification, not
the source classifier alone, is required for these promotions.

### Scene address and boundary audit

`scene_e01` now uses the same loader-derived base `0x8018EFE8`.
The jumps at file offsets `0xB8` and `0xD0` target the return at `0x140`
only with this mapping. Its actor-search handler at `0x9C` is now 172
bytes of matching C, reused from the corresponding room_m269 handler.
The header is data, and the return at `0x5E4` belongs to the preceding
stack-restoring function rather than an independent empty function.
Entry names follow the corrected addresses; embedded data addresses are
unchanged. The whole scene retains SHA-1
`af522483e6c8a6fee3a0c45f0484450e2cfefcf4`.

`scene_e19` uses file-offset-zero VRAM `0x8018EFE8`, not `0x8018EFF0`.
`Boot_InitMemoryLayout` subtracts eight from `g_StrFileDirBuffer` for
`g_LoadedSceneAssetBlock`, which `Scene_LoadRoom` uses as the section load
destination. Treating the header as code loaded eight bytes later made
absolute jumps appear to target the wrong instructions. A whole-binary SHA
match did not detect that interpretation error.

Offsets `0xC2C`, `0x19B0`, `0x2554`, `0x3144`, `0x3578`, and `0x3B20`
are return epilogues following stack restoration, not separate empty
functions. They now belong to their preceding functions and receive no
independent function credit. The real notify entries are `0xC34` and
`0x19B8`. Correct control flow removes the fabricated notify fade bug,
argument-dispatch fallback callback, and HandlerB reset-on-arm behavior.
Two 136-byte state dispatchers now use the existing plain-C macro without
pins, barriers, extra flags, or modified tools. Their full overlay retains
the original SHA-1. Other scene mappings still require individual audits;
this repair does not certify all scene function boundaries.

The direct-source debt counter gains one `extern` because the existing
`g_PlayerEntity` declaration moves out of the reset-on-arm include wrapper.
It does not introduce a new global. The existing HandlerB GTE operations and
constraints remain; this change is not a claim that HandlerB is crutch-free.

With that mapping fixed, scene_e19's HandlerC (1148 bytes), HandlerD (1352),
HandlerE (1420), and ResetSignalWithTargetGate (156) reproduce their complete
retail ranges through the existing C implementations. No additional split
boundaries were introduced. HandlerC and HandlerE reuse their reconstructed
state types; HandlerD still uses the existing offset-based state view. Their
shared GTE macros, register pins and compiler barriers are inherited matching
constraints, not newly reconstructed algorithms or crutch-free code. The
reset helper uses ordinary C without assembly constraints. The steering
adapter only converts the common state-prefix pointer for the existing
RoomLib_FxNotify2 prototype; it does not emit an assembly wrapper.

`scene_e02` has now passed the same mapping repair and whole-overlay retail
SHA check. Its six false epilogues were at offsets `0x370`, `0x20AC`,
`0x2E30`, `0x39D4`, `0x45C4`, and `0x49F8`. Two state dispatchers, handlers
C/D/E, and ResetSignalWithTargetGate add six genuine C functions (4348
bytes), while those six epilogues lose their independent credit. No code
or data addresses embedded in the retail image were rebased to force a match.

The corrected mapping also exposes two additional control-flow errors in
existing C: the triggered-effect timer must skip its decrement stores after
activation, and the mode-config setter writes its last field pair only for
mode 2. The timer now has a local C implementation because changing its
shared include would affect unaudited scenes. Stock-compiled, fully linked
retail-range trials removed two of its seven inherited pins and one of its
two laundering barriers. The five remaining pins, one laundering barrier,
and eight-byte frame allocation remain matching constraints; removing each
in the tested sequence did not match. The direct-source debt counter gains
five pins and two extern declarations as existing include content becomes
visible, not because new register constraints were introduced. The common
timer include and other scene variants remain audit work, not certified C
semantics solely because an older build passed a SHA check.

`scene_e09` and `scene_e10` independently pass their different retail SHA
checks at the corrected `0x8018EFE8` base. Each loses six false epilogue
entries (`0x39C`, `0x2604`, `0x3388`, `0x3F2C`, `0x4B1C`, `0x4F50`)
and gains the same six complete C implementations: two dispatchers,
HandlerC/D/E, and ResetSignalWithTargetGate. This is twelve newly compiled
function occurrences, not twelve unique algorithms. Their combined 8696
bytes of new C replace 96 bytes of false epilogue credit, yielding 8600
additional credited bytes and no net increase in function count.

Both scenes required the notify fade-path, HandlerB reset-on-arm and
argument-parser fallback corrections already established in scene_e19.
Their timed-fade source definitions now agree with the real entry address
in the manifest. Shared implementations and tool settings are unchanged;
no new pins or barriers were added. The direct-source extern counter rises
by two because the existing g_PlayerEntity declarations become visible
outside the old HandlerB include wrapper. This batch does not certify
the remaining unreviewed scene mappings or remove inherited constraints
from the shared GTE handlers.

`scene_e08` completes the original ten-dispatcher investigation with its
corrected `0x8018EFE8` mapping. It also exposes a gap in the earlier boundary
audit: leaf returns can be false functions even without a stack restore.
Offsets `0x6694`, `0x73D4`, `0x7E68`, and `0x8980` are common returns
reached from the preceding argument parsers' switch tables; `0x70D0` is the
gated reset's own return. Together with five stack epilogues (`0x6D14`,
`0x7A98`, `0x863C`, `0x922C`, `0x9660`), these ten entries lose their
independent function credit. Earlier scene audits must still be checked for
this non-stack form, particularly scene_e02's argument-parser endings.

Ten full scene_e08 functions now use C: two dispatchers, handlers C/D/E,
gated reset, and the four B/C/D/E argument parsers with reconstructed state
records. Their 6792 code bytes include the true leaf returns. The parsers'
four compiler-generated switch tables occupy their original `.rodata`
locations, not an additional copy before the overlay. The old 600-byte C
header containing raw arrays in `.text` is replaced by an explicit local
header-data slice, the compiler tables, and their four-byte alignment pad.
No generated binary data is committed. The remaining effect-update repair
removes a store on the wrong side of an absolute jump; it adds no instruction
assembly or register constraints. As in the other scenes, the old HandlerB
wrapper's g_PlayerEntity declaration becomes visible to the direct-source
counter (+1 extern). Full retail SHA verification includes both the new C
and the emitted jump tables, not merely the function instruction streams.

An isolated objdiff report of the retained old scene_e08 header object
confirmed that it contributed 13 inferred functions and 568 code bytes to
the denominator. The source-quality gate had already denied those arrays
decompilation credit, but it did not remove them from the total. Classifying
the header as data therefore removes those 13 pseudo-functions in addition
to the ten epilogues. This denominator repair is separate from the 6712-byte
net increase in credited C; it must not be presented as newly decompiled code.

The project uses the following independent review levels:

1. **Configured** — the binary range and source/assembly representation exist
   in a committed splat manifest.
2. **Matched** — the stock compiler pipeline produces the retail bytes. Only
   `make check` or the corresponding overlay check proves this.
3. **Semantic C** — control flow and ordinary CPU work are C; no whole-function
   assembly substitutes for reconstruction. Named GTE hardware operations are
   allowed under `ASM_AND_GTE_POLICY.md`.
4. **Crutch-free** — no register pins, empty barriers, symbol aliases, ordinary
   inline assembly, directives, or post-build rewrite exists.
5. **Header-integrated** — cross-unit ABI declarations have one owner under
   `include/`; the C file has no local `extern` declarations.
6. **Typed** — known records and fields use shared types rather than `void *`,
   byte-pointer arithmetic, or unexplained offsets.
7. **Named and organized** — symbols describe supported roles and the file
   boundary follows `CODE_ORGANIZATION.md`.

Levels 3–7 are not implied by a byte match. Reviews and progress summaries
should say which level changed. The committed debt baselines are ratchets. CI
rejects both increases and stale baselines after an improvement, independently
in `main` and `overlays`. After removing debt, run `make debt-baseline` and,
for opaque file renames, `make organization-baseline`; commit the lowered JSON
with the change. This prevents a later change from silently spending an
earlier improvement.

C files under `candidates/` are explicitly below level 2. They may be semantic
and well typed, but are not reported as matched until promoted into the
manifest and `src/` with byte verification.

### Packed field vertex transformation

`FieldEng_TransformPackedVertex` at `0x800C7AE0` is 192 bytes of stock
GCC 2.7.2 C, with no pins, compiler barriers or instruction ASM. Four unsigned
halfword counts describe two groups of 12-byte records and two groups of
16-byte records after a 16-byte geometry header. Their semantic record kinds
remain unknown. The vertex-array byte offset wraps to 16 bits before selecting
an 8-byte vertex; `ApplyMatrixSV` rotates it, then each matrix translation
component is added with 16-bit output truncation. The output padding is untouched.

The extracted range is the final function of the remaining ASM region
`0x800C71E4..0x800C7BA0`. This establishes a function boundary, not an original
translation-unit boundary. The packed-asset view lives in `render_object.h`;
the exact existing SDK function declaration lives in `gte_types.h`.

The linked function matches all 192 retail bytes. A scratch MIPS execution
check covered 4096 cases of offset wrapping, helper arguments, translation
wrapping, padding, stack and callee-saved registers, with `ApplyMatrixSV`
intercepted to supply known rotated coordinates.

### Field look angles

`FieldEng_CalculateLookAngles` at `0x800CFAA8` reconstructs 212 retail bytes
with stock GCC 2.7.2 and the default MASPSX pipeline, without pins, barriers,
or instruction ASM. It computes yaw as a quarter-turn minus `Gte_Atan2(dz, dx)`,
then pitch from the vertical difference and `Gte_ISqrt(dx*dx + dz*dz)`.
Unsigned products preserve the retail 32-bit wrapping of the squared distance.
Both angles are masked to 12 bits, roll is zero, and vector padding is untouched.
The yaw store remains before the vertical-coordinate loads, including when
output aliases an input vector. A scratch test compares host C with original
MIPS across 4096 cases, including both input aliases, with controlled SDK
helper results and exact checks of helper arguments and vector bytes.

The extracted range `0x800CFAA8..0x800CFB7C` is a function boundary within the
remaining field-engine ASM, not evidence of a separate original translation
unit. SDK math declarations share `gte_types.h`; the game API is declared
with the other render transforms in `render_object.h`.

### Field point history

`FieldEng_UpdatePointHistory` at `0x800D3AFC` matches all 204 retail bytes
using stock GCC 2.7.2 and the default MASPSX pipeline, without pins, barriers
or instruction ASM. Reset increments the signed 16-bit count and fills XYZ
in each selected entry, leaving padding intact. The other path copies entries
backwards by two aligned words and inserts the supplied entry at the front,
including its padding. Input may overlap the history: reads stay in their
retail order rather than caching the supplied point before the shift.
`RenderHistoryPoint` records both views of this eight-byte, word-aligned data.

The range `0x800D3AFC..0x800D3BC8` is a function extraction from a larger
field-engine ASM region, not a claim about original translation units.
A scratch test compares host-compiled C with original retail MIPS over 4096
cases, including signed counts, overlapping input and padding, with no mocks.

### Field cosine effect

`FieldEng_CosineEffect` at `0x800DAB98` matches all 268 retail bytes with
stock GCC 2.7.2 and unmodified MASPSX, without pins, barriers or instruction
ASM. Mode 1 computes X from the amplitude and cosine of elapsed time divided
by duration, using signed division by 4096 (truncation toward zero), increases
Y by 16, and returns completion when the time reaches the duration. Mode 2
samples the color track and submits the draw helper with scale 128 and mode 1;
other modes return zero. `RenderCosineEffect` captures the four halfword fields.
The existing `--expand-div` option preserves the retail zero/overflow checks;
valid update inputs have a nonzero duration and representable scaled time.

The extracted function range `0x800DAB98..0x800DACA4` remains a partial
field-engine reconstruction, not a recovered original TU boundary.
A scratch differential test checks host C against original MIPS over 4096
cases, including signed divisors, rounding, Y wrapping, completion and exact
helper arguments (with controlled helper results). The color-track helper
at 0x800CF3AC remains ASM; its separate candidate is not credited as a match.

### Rotating field effect

`FieldEng_RotatingEffect` at `0x800DF87C` matches all 308 retail bytes with
stock GCC 2.7.2 and unmodified default MASPSX, without pins, barriers or
instruction ASM. Mode 1 returns completion at time 24. Mode 2 clears the
render flag, obtains the current entity position, subtracts 300 from Y,
samples a color track, and submits rotation and equal sine-based X/Y scales.
Other modes return zero. Valid draw times are nonnegative and keep the
shifted time representable as a signed 32-bit value.

The existing `GteShortVector`, `GteRotation` and battle entity declaration
supply the ABI. The draw helper passes the rotation to `RotMatrixYXZ` and
uses its fourth halfword to select `MulRotMatrix`, supporting the rotation
view rather than treating those eight bytes as anonymous stack storage.
The extracted range `0x800DF87C..0x800DF9B0` is a function boundary within
remaining ASM; the original TU boundary is not yet established.

A scratch differential test compares host C with original retail MIPS across
4096 cases, including the 23/24 completion boundary, position truncation,
rotation, scale, render flag and all eleven draw arguments. Position, color,
sine and draw helpers use controlled test implementations; full executable
byte identity remains the final acceptance check.

### Field point emitter

`FieldEng_PointEmitter` at `0x800DF9B0` matches all 368 retail bytes with
stock GCC 2.7.2 and unmodified default MASPSX, without pins, barriers or
instruction ASM. Mode 0 seeds the angle and returns the pool creation result.
Mode 1 emits a point every six ticks before time 40, updates its angle and
finishes at time 70; mode 2 copies the current battle actor position.

`FieldAnimEmitter` describes the angle state; `FieldAnimEmittedPoint` names
only the accessed prefix of each 16-byte payload. The shared task slot and
callback declarations replace a local untyped pointer and correct the pool
creation callback argument. That existing function retains identical bytes.
The range `0x800DF9B0..0x800DFB20` establishes a function boundary, not a
recovered original TU boundary.

A scratch differential test compares host C with original retail MIPS across
4096 cases, including timing boundaries, exhausted pools, random values,
callback identity, return values and untouched payload bytes. Helpers use
controlled implementations; full executable byte identity is the final check.

### Drifting field effect

`FieldEng_DriftingEffect` at `0x800DC910` matches all 368 retail bytes with
stock GCC 2.7.2 and unmodified default MASPSX, without pins, barriers or
instruction ASM. Mode 1 adds independent random X/Z offsets in -3..4,
subtracts 2..5 from Y, then returns completion at time 32. Mode 2 samples
color, sets Z rotation to time * 24 and equal scales to 8192 + time * 128,
selects the CLUT row and submits the effect. Other modes return zero.
Valid draw times are nonnegative and keep the signed arithmetic representable.

The existing vector and rotation layouts supply the geometry ABI. The draw
helper reads the color's R/G/B bytes separately, supporting `RenderColor`
rather than an opaque word; this structure also reproduces the retail stack
layout. Its fourth byte is the packed color command byte. `GetClut` retains
the unsigned-short return type of its existing C definition.
The extracted range `0x800DC910..0x800DCA80` is a function boundary inside
remaining field-engine ASM, not a recovered original TU boundary.

A scratch differential test compares host C with original retail MIPS over
4096 cases: the 31/32 completion boundary, random calls, halfword wrapping,
untouched vector padding, palette branches, rotation, scale and all draw
arguments. Helpers use controlled implementations. The nearby sequence
emitter candidate at 0x800DF6AC still has six differing instruction words
and remains outside the build and matched-code totals.

### Field vector rotation through GTE

`FieldEng_RotateVector` at `0x800CEAE8` matches all 164 retail bytes with
stock GCC 2.7.2 and unmodified default MASPSX. It loads the packed rotation
matrix, clears GTE translation, transforms the input with SF=12, and narrows
MAC1/2/3 to the output halfwords. The output padding is untouched; input and
output may alias. It leaves the supplied rotation and zero translation in
GTE control registers.

Every COP2 instruction uses an existing individual macro in `pe1/gte.h`.
The two hazard NOPs are likewise separate existing macros. Ordinary loads,
zero stores and output conversion remain in C; no grouped transfer macro
hides CPU loads. `GteMatrixWords` supplies the packed control-register ABI.
The 48-byte frame holds a result vector and a partial local matrix whose only
initialized and accessed fields are its zero translation words.

Three transfer-register pins ($12/$13/$14) and two empty barriers remain in
the debt baseline, as permitted by the project's compiler-constraint policy.
Removing the pins individually gives 36, 6 and 4 differing instruction words;
removing the store-order barrier gives 6 differences, and removing the
pointer barrier shortens the function by four bytes (26 differing words).
Volatile qualifiers were removed with no byte change. The barriers preserve
zero stores before matrix loads and the local matrix-base addressing.

A scratch test compares host C with retail MIPS across 4096 cases using a
COP2 model for transfers and MAC arithmetic: full-range coefficients and
vectors, aliasing, truncation, padding, control registers and MAC results.
The model does not validate GTE flags or IR saturation; full linked byte
identity covers the actual instruction stream. The extracted range
`0x800CEAE8..0x800CEB8C` is a function boundary, not an established original TU.
The sine-drift candidate at 0x800DC5BC remains outside the build with four
differing instruction words.

### Transforming the GTE translation

`FieldEng_TransformTranslation` at `0x800CF5B0` matches all 168 retail bytes
with stock GCC 2.7.2 and unmodified default MASPSX. It loads the current
matrix's rotation and translation, transforms the input vector with SF=12,
writes MAC1/2/3 into the output matrix's translation, and reloads those words
into TRX/TRY/TRZ. A null output uses a local matrix; its rotation is neither
initialized nor accessed. A supplied output retains its rotation words and
may alias the current matrix.

The existing `RenderMatrixSlot` declaration moves unchanged from
`Render_DecompressAnimFrame.c` to `pe1/render_object.h`; that function's text
remains byte-identical. The slot's s32 pointer is viewed through the existing
packed `GteMatrixWords` ABI. Every COP2 transfer, command and hazard NOP uses
an existing individual macro; ordinary CPU loads and pointer handling stay
in C. No compiler or assembler changes are involved.

Four pins ($3/$12/$13/$14) and one empty pointer barrier are recorded in debt.
The barrier preserves the separate address of the matrix slot. Removing the
matrix pin produces nine differing instruction words; removing the transfer
pins individually produces 39, eight and six differences. Removing the
barrier shortens the function from 168 to 160 bytes. These are compiler
constraints, not evidence that the original function was handwritten ASM.

A scratch differential test compares host C and retail MIPS over 4096 cases
with modeled COP2 transfers and MAC arithmetic: full-range coefficients,
vectors and translations, null output, current-matrix aliasing, preserved
rotation/input bytes and final control/MAC registers. GTE flags and IR
saturation are not modeled. Full linked byte identity verifies the complete
instruction stream. The range `0x800CF5B0..0x800CF658` is a function boundary;
the original TU boundary remains unresolved.

### Transforming a point by an indexed object matrix

`FieldEng_TransformMatrixPoint` at `0x800CE8F0` matches all 228 retail bytes
with stock GCC 2.7.2 and unmodified default MASPSX. It selects a 32-byte
matrix from the owner's table, loads its rotation, clears GTE translation,
and transforms the input with SF=12. CPU-side unsigned addition applies the
selected matrix's translation modulo 32 bits before narrowing to output
halfwords. This preserves retail wrapping without signed-overflow undefined
behavior. The output padding is untouched; input and output may alias.

The existing `RoomFxTransformOwner` supplies the table pointer at +0x238.
Its `RoomFxTransform` entries already describe the translation at +0x14;
`GteMatrixWords` supplies the packed rotation view. No competing owner layout
is introduced. The function reloads the owner's table for each output
coordinate, as in retail, and leaves the selected rotation and zero
translation in GTE control registers. Every COP2 transfer, command and hazard
NOP uses an existing individual macro; ordinary loads and arithmetic stay in C.

Three transfer pins ($12/$13/$14) and one empty pointer barrier remain in
debt. The three scalar `(u32)` conversions are also counted by the current
syntactic `pointer_integer_casts` rule; they convert signed MAC values, not
pointers. Removing the pins individually gives 54, six and four differing
instruction words; removing the barrier shortens the function to 224 bytes
and gives 42 differing words. The barrier retains base-plus-field addressing
for the local zero-translation matrix. The partial local matrix's other
words are neither initialized nor accessed.

A scratch differential test checks host C against retail MIPS over 4096
cases using modeled COP2 transfers and MAC arithmetic: valid positive and
negative indices within an array, full-range coefficients/vectors/translations,
input/output aliasing, narrowing, padding, unchanged owner/matrices and final
control/MAC registers. GTE flags and IR saturation are not modeled; full
linked byte identity verifies the actual instruction stream. The extracted
range `0x800CE8F0..0x800CE9D4` is a function boundary, not a recovered original TU.

## Migration order

For a subsystem, prefer this order:

1. establish binary and probable object boundaries;
2. centralize exact existing declarations without changing ABI;
3. introduce shared partial structures for repeatedly accessed offsets;
4. replace address names only when call/data evidence supports a role;
5. remove compiler crutches with byte verification;
6. consolidate files only after boundary evidence is recorded.

This ordering keeps cleanup reviewable and avoids hiding ABI changes inside
large mechanical source moves.

### LIBCARD interrupt patch continuation

`CardPatchFunctions` at 0x8007E370 now matches all 68 retail bytes without
instruction ASM. This is a copied fragment of LIBCARD/PATCH, not a public
Psy-Q API or a newly established original translation unit. Its former C
source had an uninitialized pointer and an inline-ASM continuation jump.
The incoming I/O base is now an explicit global v1 register binding, and
`CardPatchIoRegisters` names the observed 0x1044/0x1074 word accesses. Their
mapping to serial status and interrupt mask agrees with the
[PSX I/O map](https://psx-spx.consoledev.net/iomap/).

The fragment returns when interrupt-mask bit 0x80 is clear; otherwise it
waits for serial-status bit 0x80 to clear, then jumps through the continuation
slot at 0xDFFC. The adjacent installer at 0x8007E3DC writes that slot after
copying its patch. A GNU computed goto expresses the BIOS continuation without
changing ra. This is intentionally a compiler-specific BIOS patch boundary,
not portable ISO C or an ordinary call to another C function.

Stock GCC 2.7.2 and unchanged upstream MASPSX produce an exact 68-byte linked
match. There are no empty barriers or explicit NOPs. The one v1 binding is
part of the entry ABI: removing it adds three pointer-load instructions and
produces 80 bytes instead of 68. Debt records one additional pin and computed
goto, with one fewer instruction-ASM unit. The existing manifest boundary
and historical name are retained; this repairs a constrained C unit rather
than discovering another library export.

A scratch Unicorn check covers 2048 differential cases (4096 executions) at
retail and relocated patch addresses, with randomized interrupt masks and
status words and zero to 31 busy polls. It checks ordered 32-bit reads, both
exit destinations, unchanged v1/sp/ra, and no stores. Status is modeled input;
this is not a full BIOS or serial-hardware timing test.

Clean production verification passes all 340 tests and the main retail SHA-1;
all 191 overlay binaries also retain their retail SHA-1.

### LIBCARD installed-patch redirect

`func_8007E3B4` is now a 16-byte C redirect, appended to the existing
`CardPatchFunctions` unit. The combined 84-byte C range matches retail.
There are no new pins, empty barriers, NOP macros or instruction ASM; the
GNU computed goto preserves the BIOS return address and incoming v1. It
enters the uncached address 0xA000DFAC, which is the previous wait fragment
copied to 0xDF80 + 0x2C by `_copy_memcard_patch`. This is an internal copied
patch entry, not a newly identified public Psy-Q API.

The installer at 0x8007E3DC copies 0x8007E3B4..0x8007E3C8: 16 bytes of
redirect instructions and one trailing zero word. The other installer at
0x8007E470 copies the separate 0x8007E3C8..0x8007E3DC template. These two
copy ranges establish a boundary that the old 36-byte disassembly function
crossed. The first padding word is now an explicit pad segment; the second
template and both installers remain assembly. This fixes an inferred boundary
as well as adding 16 matching C bytes; changes in the total function count or
padding denominator are not additional decompilation progress.

The shared LIBCARD header declares the new entry as a function. The existing
copy helper converts its address to the instruction-word end pointer, instead
of redeclaring the function as an array. Its object instructions are unchanged.
The installed uncached address is declared in the manual linker symbols,
since it lies outside the executable's own image. One new computed goto and
one fewer C-file extern are recorded in debt.

The redirected path passes 2048 differential cases (4096 Unicorn executions):
both retail and relocated redirect entries execute the installed wait fragment
at its uncached address. Assertions cover ordered status reads, return versus
BIOS continuation, no stores, and preservation of v1/sp/ra. Serial status is
modeled input, not a full device simulation. Clean validation passes 340 tests
and the complete main retail SHA-1.
All 191 overlay binaries also retain their retail SHA-1.

### LIBMATH double multiplication

`Math_Sqrt64` is the historical project symbol for Psy-Q `__muldf3`, not a
square-root operation. Its 788 bytes at 0x800735C4 now match retail with stock
native GCC 2.7.2 and the existing `ASSEMBLER: GNU` path in `cc.sh`. Neither the
compiler nor the assembler pipeline was changed. The implementation reconstructs
mantissa partial products, rounding and normalization, sign packing, and the
original overflow event and infinity result. It preserves the library's actual
edge-case behavior rather than replacing it with host floating-point arithmetic.

Two input exponent variables let GCC retain their common sum as an unnamed
value and spill it to sp+80. The alternate adjusted exponent is calculated in C
after the ten-bit shift call; GCC schedules the arithmetic before that call and
reloads the spilled sum between the two mantissa argument loads. A separate
`MathDoubleBits` result also gives the original final low-word copy. This
replaces the unsuccessful explicit stack field and both fixed t3 variables.
There are no register pins, loops, NOP macros or instruction ASM.

One tied empty barrier keeps the initial adjusted exponent independent of the
shared sum. Removing it produces 784 bytes instead of 788, with 95 differing
instruction words in a linked comparison. The old final memory barrier and
both pins are absent. Temporary reuse in the C expressions is retained where
simplifying it changes allocation; the unused mask alias, unused local and
redundant scopes were removed while preserving byte identity. Debt increases
by exactly one empty barrier.

The source uses the shared math and event declarations, including the actual
integer return type of `Evt_Deliver`. A differential check executes 2169 input
pairs against retail with the original integer helpers, comparing result bits,
overflow events, stack pointer and callee-saved registers. This includes zero,
subnormal, exponent-boundary and randomized bit patterns; byte identity verifies
the complete instruction stream independently of those sampled cases.

The SDK provenance inventory identifies a 1024-byte MULDF3 object containing
`__muldf3` and the adjacent `_mul_mant_d` (`Math_Mul32To64`). The latter's existing
236-byte match still uses a separate GCC 2.8.1 unit. The new `muldf3.c` therefore
covers the 788-byte function subrange, not a newly inferred full object boundary.
The obsolete non-matching candidate was removed; the earlier false-match archive
remains historical evidence. The report's address-based SDK mapping retains the
correct LIBMATH/MULDF3/__muldf3 identity.

Clean production verification passes all 340 tests and the complete main retail
SHA-1. All 191 overlay binaries also retain their retail SHA-1.

### Polygon containment reconstruction

`Geo_PointInPoly` at `0x8001CAB0` is reconstructed as C, with a complete
240-byte linked match under stock GCC 2.7.2 and unchanged MASPSX. It tests
integer halves of 16.16 X/Z vertices by counting horizontal ray crossings.
The shared `PolygonVertex` declaration identifies the eight-byte record and
its signed halfwords at offsets 2 and 6; callers now share its prototype.
The original requires at least one vertex. Cross products explicitly keep
the low 32 bits using widened multiplication, preserving retail behavior
without signed multiplication overflow in C.

Two local register bindings remain: the inside accumulator (`t3`) and the
edge comparison result (`v0`). Removing them individually changes 10 and 3
instruction words; removing both changes 13. An 800-variant declaration,
type and temporary-reuse search found no unpinned exact match. There are no
empty barriers, instruction ASM, explicit NOPs, EABI or compiler changes.
Both pins are recorded as matching debt, not clean-C progress.

The existing `main/geo` assembly range is split only at the function end,
`0x8001CBA0`; the remaining functions stay together. This is an incremental
matching boundary, not evidence of an original source-file boundary.
The C algorithm passed 6000 host cases against an independent rational
ray-intersection reference, with randomized fractional coordinate halves.
Host execution removes only the two MIPS register-placement constraints.
Experiment sources, full-byte comparisons and the host checker are under
`/tmp/pe-point-in-poly/`.

Clean-build validation preserves the complete main retail SHA-1 and all 191
overlay SHA-1 values. `make verify` passes all 340 tests and the source,
organization and debt gates. This adds one game function (240 code bytes),
not a Psy-Q function.

### Collision response reconstruction

`Entity_ApplyCollisionResponse` at `0x8001D170` now matches all 248 retail
code bytes using stock GCC 2.7.2 and unchanged MASPSX with `-G8`. It publishes
the collision half-width, queries the nearest polygon edge, attempts a wall
slide, and restores the saved X/Y/Z position if the second query still reports
an edge. The incoming argument is unused in retail; the routine reads the
shared actor slot instead. Signed 16-bit narrowing of the query results and
the low-word multiply followed by signed division are preserved explicitly.

The existing `BattleEntity` fields describe all actor accesses. Separate
locals preserve the post-call reloads of the shared actor pointer. There are
no register bindings, empty barriers, NOPs, instruction ASM or EABI.

The four zero bytes at `0x8009D254` are now a real C pointer definition in
`.data`, with the compatible declaration already in `battle_runtime.h`.
The existing `g_PlayerEntity` linker alias still identifies the same slot.
This definition also reproduces the retail non-GP accesses to that symbol,
while the three collision-query globals retain GP-relative accesses. A plain
external pointer declaration under GCC 2.7.2 and `-G8` selected GP-relative
loads instead. No padding struct, symbol-type alias or assembler rewrite is
used to force the addressing. The four data bytes are not counted as new code.
The incremental boundary retains the two remaining `main/geo` functions in
assembly; it does not claim an original source-file boundary.

A 512-case host test runs the production function with mocked edge/slide
helpers and a test actor view retaining the accessed retail offsets. It covers
all exit paths, wrapped products, signed result narrowing, saved-position
arguments and changes of the shared actor pointer between calls. ASan and
UBSan pass. Only ELF section placement is disabled for the Mach-O host build;
the algorithm is unchanged. Scratch sources and byte comparisons are in
`/tmp/pe-collision-response/`.

Clean-build validation preserves the complete main retail SHA-1 and all 191
overlay SHA-1 values. `make verify` passes all 340 tests and the source,
organization and debt gates. This adds one game function (248 code bytes),
not a Psy-Q function, and introduces no matching debt.

### Stream sample block reconstruction

`Spu_UploadStreamBlockB` at `0x800875FC` is semantic C matching all 272
retail bytes with stock GCC 2.7.2 and unchanged MASPSX. It belongs to the game's
AKAO sound engine, not Psy-Q. The function validates bank 0/1 and the sample
header, locates the sample payload after the instrument records, uploads it,
rebases instrument addresses, then copies the relocated records to the stream
scratch table. An encoded exclusive instrument end of zero means 256.

The reconstruction adds no register bindings, barriers, NOPs, instruction ASM,
EABI or compiler changes. Reusing the transfer-size variable as the later word
count and the SPU-address variable as the later scratch-table offset reproduces
the retail allocation without constraints. Header words are read through a
32-bit cursor; this is an incremental function boundary, not evidence of an
original translation-unit boundary. Shared declarations live in `pe1/akao.h`.

A native test of the production C passes 323 cases under ASan/UBSan, covering
both banks, 1–16 records, the zero-end encoding, invalid banks and invalid
headers. Mocked transfer helpers check call order and arguments; the relocation
stub changes record addresses before the test checks every copied word and
untouched scratch-table word. The test does not exercise SPU hardware. The
linked byte comparison and test harness are in `/tmp/pe-stream-block-b/`.

`make verify-clean` passes all 340 tests and the source, organization and debt
gates. The complete main image retains retail SHA-1
`452fb033f2eaa4b18aa20a5bca60b8125af3a37b`; all 191 rebuilt overlays also retain
their retail SHA-1 values.

### Glyph slot lookup reconstruction

`Render_DrawTextDigit` at `0x80038BC0` matches all 292 retail bytes with stock
GCC 2.7.2 and unchanged MASPSX. Despite its historical name it draws nothing:
it selects a code from the mode and font state, searches the code list, then
returns the corresponding slot index or `0xFF`. A missing code uses code-list
index zero, preserving the retail fallback. A null input uses `D_80091A28`.
This is game code, not Psy-Q.

`pe1/font.h` describes the observed table layout: the first count at +3,
codes at +4, the second count at +0x1C and trailing indices at +0x1D. The
unknown prefix bytes remain unnamed; the 24-byte codes region is inferred
from the next count's fixed offset. This partial layout is not a claim that
the original font format or translation-unit boundary has been identified.

**Manual matching debt: one unused local array (`stack_pad[2]`).** It retains
the retail 16-byte leaf stack frame; the original local variables are unknown.
The automated debt tracker currently does not count unused stack padding,
so an unchanged numerical baseline must not be read as zero new debt. There
are no register pins, compiler barriers, NOPs, instruction ASM or EABI here.

The production C passes 131072 native cases under ASan/UBSan against an
independent two-search reference: all byte-valued modes and font states,
explicit and default table pointers, generated list lengths including empty
lists, absent codes, repeated entries and unchanged input data. The harness
checks the recovered field offsets too. Scratch proof and linked byte
comparison are in `/tmp/pe-font-slot/`.

`make verify-clean` passes all 340 tests and the source, organization and
tracked-debt gates. The complete main image and all 191 rebuilt overlays
preserve their retail SHA-1 values. The manual stack-frame debt above remains.

### Equipment modifier loading reconstruction

`BattleCmd_LoadWeaponModifiers` at `0x80051CC4` matches all 308 retail code
bytes with stock GCC 2.7.2, unchanged MASPSX and `-G8`. The C switch also
reproduces all 32 bytes of its jump table at `0x800111F8`; the manifest now
places that compiler-generated `.rodata` at the original address. This is one
new game function, not Psy-Q; the jump table is data, not added code progress.

Despite the historical weapon-oriented name, the lookup reads the signed
equipped armor index at `0x800C0E22`. It saves the active-list selection,
selects the normal list, clears the modifier mask and seven modifier words,
then interprets the low five bits of each `ItemDataRecord.tailData` entry.
Later entries overwrite earlier values in the same output slot; mask values
are assigned, not accumulated with OR. The stat recalculation and both final
list-selection calls remain in their retail order even for a missing record.

The existing item record supplies the count and modifier fields. The seven
output words are represented as an array, matching the descending clear loop.
The armor index retains the existing signed-byte-array view used by command
rollback code, which preserves non-GP addressing under GCC 2.7.2; migrating
these older save-state views to `AyaSaveState` remains type debt. No fake
padding structs, new symbol aliases, register pins, compiler barriers, NOPs,
unused stack arrays, instruction ASM or EABI are introduced. Shared interfaces
are in `pe1/battle_modifiers.h`; the incremental split does not establish an
original translation-unit boundary.

The production C passes 8192 native cases under ASan/UBSan against a separate
modifier reference, including every encoded modifier byte, all signed byte
indices, missing/empty records and mixed modifier sequences. Mocked inventory
helpers check clearing before lookup, final outputs, call order and restoration
of the saved selection. Item records remain unchanged. The test does not
exercise the inventory helpers themselves. Scratch tests and linked code/data
comparisons are in `/tmp/pe-weapon-modifiers/`.

`make verify-clean` passes all 340 tests and the source, organization and debt
gates. The complete main image and all 191 rebuilt overlays retain their
retail SHA-1 values.

### Color-track interpolation reconstruction

`func_800CF3AC` at `0x800CF3AC` now matches all 264 retail bytes in
`engine/FieldEng_InterpolateColorTrack.c`, using stock GCC 2.7.2 and unchanged
MASPSX. It initializes cached duration/start halfwords for a sentinel-terminated
RGB track, records its total time and segment count, clamps time at the end,
finds the applicable segment backwards and calls `LoadAverageCol` with two
12-bit complementary weights. The existing symbol and void-pointer API remain
compatible with the effect callers. This is game code, not Psy-Q.

`RenderColorKey` is eight bytes: RGB plus an encoded one-byte segment duration,
followed by two cached timing halfwords. `RenderColorTrack` has an eight-byte
header and trailing keys. The zero-duration terminal key supplies the final
RGB value; its timing halfwords are not initialized. Valid inputs have at least
one positive-duration segment and nonnegative time. No new validation is added
for malformed tracks; retail's division checks are preserved by the existing
MASPSX `--expand-div` option.

Matching debt is one `$8` count pin, recorded in the numerical baseline. The
byte cursor into timing fields, including the preceding duration-byte read,
also remains manual source-shape debt: its intermediate view reproduces the
retail addressing. The automatic pointer-arithmetic patterns do not count all
these `unsigned char *` expressions. There are no empty barriers, NOPs, unused
stack arrays, instruction ASM, EABI or tool modifications. Keeping the header
writes inside the terminating branch reproduces the initialization loop shape;
pins alone did not resolve the earlier rotated loop.

The production algorithm passes 5120 native cases under ASan/UBSan with 1–128
segments, uncached/cached paths, boundary times and end clamping. A mocked
`LoadAverageCol` verifies the two selected keys, output pointer and both weights;
checks also cover every cached timing value and the untouched terminal timing.
The host build suppresses the PSX register binding and unrelated 32-bit-pointer
layout assertions, then checks the new pointer-free layouts explicitly. It does
not emulate GTE arithmetic. Scratch proof is in `/tmp/pe-color-track/`.

`make verify-clean` passes all 340 tests and the source, organization and
updated-debt gates. The complete main image and all 191 rebuilt overlays
preserve their retail SHA-1 values.

### Save notification drawing reconstruction

`Menu_SaveOverlayDraw` at `0x80034DE0` now matches all 304 retail bytes with
stock GCC 2.7.2, unchanged MASPSX and `-G8`. State 1 resets the text tables,
chooses the cursor rectangle from the metadata window and layout selector,
sets the color table, then activates the first textbox with the message and
flag `0x02000000`. State 2 resets the tables when its timer is zero. Every
call decrements the byte timer and draws the background strip, including
inactive states: zero wraps to 255, and a newly assigned 75 becomes 74.

The existing `TextboxEntry` supplies the state, message and flags fields;
there are no raw textbox offsets or new layout aliases. The state and message
are reloaded after the setup helper, preserving callback-visible updates.
The existing function name and retail global names are retained; no original
translation-unit boundary is claimed. This is game code, not Psy-Q.

Matching debt is one `$4` state pin and one empty memory barrier, both recorded
in the baseline. The barrier preserves the message load after activation and
timer stores. No instruction ASM, NOPs, unused stack padding, EABI or compiler/
assembler modifications are introduced. Shared declarations live in the save
and textbox subsystem headers.

The production algorithm passes 262144 native cases under ASan/UBSan:
all 256 states and timer values across layout selectors 0, 1, 2 and 255.
Mocked helpers check call order, coordinates, color arguments, final textbox
contents, timer wrapping and state/message changes during setup. The host
build suppresses the register binding and empty barrier; the test does not
exercise actual text rendering or helper implementations. Scratch source,
harness and linked comparison are in `/tmp/pe-save-overlay/`.

`make verify-clean` passes all 340 tests and the source, organization and
updated-debt gates. The complete main image and all 191 rebuilt overlays
preserve their retail SHA-1 values.

### Equipment confirmation callback reconstruction

`Menu_OnEquipConfirm` at `0x80046574` matches all 332 retail bytes with stock
GCC 2.7.2, unchanged MASPSX and `-G8`. The callback ignores unconfirmed input.
On confirmation it dispatches the selected grid cell to the item-use helper
or the existing status-returning `Inv_GetSlotItemData` operation. A failed
operation opens notification `0x1D`; successful paths hide the selection
cursor and transfer focus to panel 5 or 6. It then rebuilds the filtered list,
resets the cursor of panel 6 when focused, and pauses polling in item-use mode.

The existing `MenuWidgetNode` supplies both cursor fields. The mode global
is reloaded after helper calls, as in retail. Selecting the focus node with
an expression directly in `MenuWidget_SetCurrentNode` preserves the original
branch layout without register pins, compiler barriers, instruction ASM,
unused stack padding or extra compiler flags. Shared declarations are in the
menu-inventory and inventory headers. The historical helper names remain;
`Inv_GetSlotItemData` returns a status despite its name. This is game code,
not Psy-Q, and no original translation-unit boundary is claimed.

The narrow linked comparison is exact for all 332 bytes. Scratch source and
comparison artifacts are in `/tmp/pe-equip-confirm/`.

`make verify-clean` passes all 340 tests and the source, organization and debt
gates without increasing debt. The complete main image and all 191 rebuilt
overlays preserve their retail SHA-1 values.

### Field task allocation reconstruction

`func_800D401C` in `FieldEng_AllocateTask.c` matches all 288 retail bytes with
stock GCC 2.7.2 and unchanged MASPSX. It scans eight task slots for the `0xFFFF`
sentinel, returns -1 when full, and otherwise assigns the task id and clears
its age. A dispatch table supplies the initial payload size and callback.
The 16-bit used-byte counter is updated before testing the `0x97C` arena
limit, preserving retail wraparound. The callback receives mode 0, the payload
and the context argument; its additional size advances the cursor and counter.
A zero callback result clears the slot's end pointer. Context fields are
reloaded after the callback, preserving callback-side updates.

`FieldAnimTaskSlot` now exposes its id and age halfwords. The shared header
also describes the context and the task-table prefix, with target layout
assertions. The neighboring initializer `func_800D4620` confirms the slot
stride, sentinel and arena start; `func_800D413C` and its update path confirm
the script cursor, delay, halfword variables and slot age. The table prefix
ends before the later lifecycle callback at +0x30. Unknown flag semantics
and the context argument's ultimate type remain unclaimed. Existing raw
context views in other translation units are not migrated by this change.

There are no pins, barriers, NOPs, instruction ASM, stack padding or custom
compiler flags. The scan counter is reused for the callback result to recover
retail register allocation. The function's existing symbol is retained; the
file name describes its observed role and makes no original TU claim.
This is game code, not Psy-Q.

The production C passes 3456 ASan/UBSan host cases covering every first-free
slot and a full pool, all eight callback indices, three payload sizes, four
counter boundaries (including 16-bit wraparound), zero/nonzero callback
results, and callback changes to the cursor/counter. The host test suppresses
only PSX layout assertions; the linked target comparison checks all 288 bytes.
Scratch comparison and harness are in `/tmp/pe-task-slot/`.

`make verify-clean` passes all 340 tests and the source, organization and debt
gates with no debt increase. Main and all 191 rebuilt overlays preserve their
retail SHA-1 values.

### Shrinking field effect reconstruction

`func_800D7E78` in `FieldEng_ShrinkingEffect.c` matches all 324 retail bytes
with stock GCC 2.7.2 and unchanged MASPSX, without extra compiler flags.
Mode 1 decrements Y and returns completion when the task age is at least 6.
Mode 2 snapshots XYZ, interpolates its color track, computes scale
`4096 - (age << 12) / 6`, selects a palette and draws texture `0x8A`.
Palette selector 4 adds four rows when the asset flag is nonzero. Other
modes return zero without work.

The source uses the existing `GteShortVector` and `RenderColor` types. Only
XYZ is copied; the vector's padding is not initialized or used by this
function. Position is captured before the color helper, while task age and
palette globals are read afterward, preserving callback-visible changes.
The historical symbol is retained, and the descriptive filename makes no
original TU claim. This is game code, not Psy-Q.

No pins, barriers, NOPs, instruction ASM, unused stack padding or compiler/
assembler modifications are needed. The production C passes 132105 native
ASan/UBSan cases: all signed halfword Y values at ages 5 and 6; rendering at
ages 0–31 with eight palettes, both asset-flag states, and callback mutations
of age and input coordinates; and inactive modes. Mock helpers check call
order, the position snapshot, color, scale and draw arguments. The host build
suppresses unrelated target layout assertions; target compilation and the
linked comparison validate the PSX binary. Scratch proof is in
`/tmp/pe-shrinking-effect/`.

`make verify-clean` passes all 340 tests and the source, organization and debt
gates without a debt increase. Main and all 191 rebuilt overlays retain their
retail SHA-1 values.

### Sine field effect reconstruction

`func_800DAF8C` in `FieldEng_SineEffect.c` matches all 324 retail bytes with
stock GCC 2.7.2 and unchanged MASPSX, without extra flags. Mode 1 adds the
stored Y velocity, advances Z by 8, decays X from its initial amplitude over
64 ticks, and updates the angle from `rsin(age << 5) / 64`. Scale decays from
1600 by `age * 1000 / 64`; completion is reported at age 64. Mode 2 blends
the color track and dispatches drawing with the current position, scale and
angle. Other modes return zero.

The shared `RenderSineEffect` describes the observed 16-byte state using the
existing position vector and four halfwords. Static assertions check its
size and field offsets. The position vector's padding is untouched. The
original symbol is retained, and the file's descriptive name makes no
original translation-unit claim. This is game code, not Psy-Q.

No pins, barriers, NOPs, instruction ASM, unused stack padding or toolchain
modifications are needed. State and globals remain observable across helper
calls: the age is reloaded after sine evaluation, and rendering reads the
scale and angle after color interpolation.

The production algorithm passes 655501 native ASan/UBSan cases. These cover
all signed-halfword amplitudes at ages 0, 1, 63, 64 and 65; positive/negative
mock sine outputs; halfword coordinate wrapping; age changes during sine
evaluation; render-state changes during color interpolation; and inactive
modes. Mock draw calls check the data, position, color and all scalar
arguments. This verifies the callback, not the sine or renderer themselves.
The host build suppresses unrelated target layout assertions and explicitly
checks the new 16-byte state size. Target compilation and the narrow linked
comparison verify the PSX layout and all 324 bytes. Scratch proof is in
`/tmp/pe-sine-effect/`.

`make verify-clean` passes all 340 tests and the source, organization and debt
gates without increasing debt. Main and all 191 rebuilt overlays retain their
retail SHA-1 values.

### Sine effect emitter reconstruction

`func_800DB0D0` in `FieldEng_SineEmitter.c` matches all 396 retail bytes with
stock GCC 2.7.2 and unchanged MASPSX. Mode 0 seeds its phase, obtains the
actor position and creates a list of sixteen 16-byte `RenderSineEffect`
payloads using `func_800DAF8C` as the callback. Mode 1 requests at most one
new payload per call through age 16, initializes amplitude, Y, Z and Y
velocity, and advances the phase. It completes at age 73. Mode 2 publishes
the emitter position and sets the depth-related global to 8.

The shared `RenderSineEmitter` exposes its position and phase, with assertions
for its 12-byte layout. `D_800E221C` is now a `GteShortVector`, replacing the
raw byte-array declaration; the previous sine effect passes its address to
the renderer. Keeping the three coordinate writes in this one vector view
recovers the original instruction order without barriers. Its padding remains
untouched. The existing symbol and generic task-callback convention are
retained; the descriptive filename makes no original TU claim. This is game
code, not Psy-Q.

There are no pins, barriers, NOPs, instruction ASM, unused stack padding or
extra toolchain flags. The production C passes 142601 ASan/UBSan host cases:
256 initialization seeds; ages 0–74 across 256 random sequences, available/
full lists and phase changes during the final random call; every halfword
coordinate value for position publication; and inactive modes. The mocked
list initializer checks stride, count and callback identity; tests also check
uninitialized payload fields remain untouched. Host tests suppress unrelated
PSX layout assertions and explicitly check the new emitter size. Target
compilation and the linked comparison validate the PSX layout and all 396
bytes. Scratch comparison and harness are in `/tmp/pe-sine-emitter/`.

`make verify-clean` passes all 340 tests and the source, organization and debt
gates with no debt increase. Main, including the previous sine callback, and
all 191 rebuilt overlays preserve their retail SHA-1 values.

### Arcing field effect reconstruction

`func_800D7A1C` in `FieldEng_ArcingEffect.c` matches all 340 retail bytes with
stock GCC 2.7.2 and unchanged MASPSX, without extra flags. Mode 1 subtracts
Y velocity from Y, then decrements that velocity, completing at age 24.
Mode 2 interpolates color, copies the current XYZ and draws texture `0x8B`
at scale 4096. Palette selector 4 adds four rows when the asset flag is set.
All calls initialize a local color from `D_800C22DC`, including inactive
modes; the four-byte `RenderColor` assignment preserves the unaligned copy
instructions seen in retail.

`RenderArcingEffect` names the observed eight-byte state: three position
halfwords and a Y velocity at +6. The last field is active velocity, not
vector padding. Shared size/offset assertions verify that distinction.
Position is copied after color interpolation, preserving helper-side updates;
only XYZ is written in the local draw vector. The historical symbol remains,
and the descriptive filename makes no original TU claim. This is game code,
not Psy-Q.

No pins, barriers, NOPs, instruction ASM, unused stack padding or toolchain
modifications are needed. Separate position and velocity assignments recover
the original memory access order. The production algorithm passes 132105
ASan/UBSan host cases: all signed-halfword velocities at ages 23 and 24,
coordinate/velocity wrapping, rendering at ages 0–31 across eight palettes
and both asset states, helper-side position changes, and inactive modes.
Mocks check the initial color copy, helper order and all draw arguments.
The host build suppresses unrelated target assertions and checks the new
state size explicitly; target compilation and the narrow linked comparison
validate the PSX layout and all 340 bytes. Scratch proof is in
`/tmp/pe-arcing-effect/`.

`make verify-clean` passes all 340 tests and the source, organization and debt
gates without increasing debt. Main and all 191 rebuilt overlays retain
their retail SHA-1 values.

### Arcing effect emitter reconstruction

`func_800D7B70` in `FieldEng_ArcingEmitter.c` matches all 776 retail bytes
with stock GCC 2.7.2 and unchanged MASPSX, without extra flags. Mode 0
seeds the phase, clears the radius, obtains the actor position and creates
24 eight-byte `RenderArcingEffect` payloads. Mode 1 emits through age 50,
using cosine/sine offsets around the stored position, randomizes Y velocity
and advances phase. Completion is reported at age 74. Mode 2 draws the
rotating central effect through age 50, updates the radius, configures the
particle parameters and requests texture upload via `func_800CEDA8`.

`RenderArcingEmitter` describes the 16-byte state. The new 18-byte
`RenderEffectParameters` describes the halfword block at `0x800F3368`;
using one structure recovers retail store ordering without pins or barriers.
Parameter names remain offset-based where their semantics are unresolved.
Existing scalar declarations in other units are still views into the same
retail storage; this change adds no definition, linker alias or claim that
all consumers have been migrated. Both layouts have target assertions.
The historical function symbol remains, and the filename makes no original
TU claim. This is game code, not Psy-Q.

No pins, barriers, NOPs, instruction ASM, stack padding or toolchain patches
are introduced. The production C passes 39784 native ASan/UBSan cases:
initialization seeds, ages 0–75, full/available lists, positive/negative
mock sine and cosine values, coordinate narrowing, random parameter masks,
eight texture-table indices, rendering cutoffs and inactive modes. Mock
helpers verify pool dimensions, callback identity, emitted state, draw
arguments and parameter values at upload time. The host test suppresses
unrelated PSX assertions and checks both new sizes explicitly; target
compilation and the linked comparison verify the PSX layout and 776 bytes.
Scratch source and proof are in `/tmp/pe-arcing-emitter/`.

`make verify-clean` passes all 340 tests and the source, organization and debt
gates with no debt increase. The complete main image and all 191 rebuilt
overlays preserve their retail SHA-1 values.

### Orbiting field effect reconstruction

`func_800D7FBC` in `FieldEng_OrbitingEffect.c` matches all 972 retail bytes
with stock GCC 2.7.2 and unchanged MASPSX. Mode 1 orbits around `D_800E21EC`,
advances angle by 96 and recomputes radius from a 36-tick cosine curve. It
emits jittered particles every tick when context variable 6 equals 11,
otherwise every third tick, increments its halfword timer and completes at
age 36. Mode 2 runs the fade-in, steady and shrinking stages, then draws
texture 36 with the selected palette and the copied constant color.

`RenderOrbitingEffect` is 16 bytes: stage, angle, timer, XYZ, radius and an
untouched final halfword. The neighboring initializer `func_800D8388`
requests 16-byte payloads and initializes the observed fields. Shared
assertions check size and offsets. The center uses `GteShortVector`; the
particle list is declared separately. Existing symbols remain and the file
name makes no original TU claim. This is game code, not Psy-Q.

Matching debt is one `$3` pin for the palette comparison constant. The
unpinned source was five instructions away after counter reuse; nested/
inverted conditions, shift forms, call-site scaling, declaration orders and
ordinary `register` hints did not recover that allocation. The pin is in the
ratchet baseline. There are no barriers, NOPs, instruction ASM, stack padding
or compiler/assembler modifications. `--expand-div` emits retail's divide
checks for the variable remainder operation; the ordinary pipeline's default
omits those checks. It does not change the C algorithm.

The production C passes 16560 ASan/UBSan host cases covering ages 0–40,
both emission intervals, available/full lists, signed mock trig values,
all random jitter masks, halfword coordinate/timer wrapping, each rendering
stage and its transition, eight palettes and both asset-flag states. Helpers
check trig arguments, particle coordinates, color and all draw arguments.
The host build suppresses the pin and unrelated target layout assertions,
then checks the new state size explicitly. The linked comparison verifies
all 972 target bytes; scratch proof is in `/tmp/pe-orbiting-effect/`.

`make verify-clean` passes all 340 tests and the source, organization and
updated-debt gates. Main and all 191 rebuilt overlays preserve their retail
SHA-1 values.


### Orbiting burst emitter reconstruction

`func_800D868C` in `FieldEng_OrbitingBurst.c` matches all 748 retail bytes
with stock GCC 2.7.2 and unchanged MASPSX. Initialization seeds the angle and
allocates three 16-byte orbiting effects plus eighteen 8-byte particles.
At age 1 it makes three allocation attempts, initializes successful effects
with radius 700 and actor Y minus 400, and steps their phases by -0x555.
It updates and publishes the particle list until age 43. Drawing transfers
eight matrix words to GTE, draws the particle list with 16-unit parameters,
updates the shared orbit center, and installs 32-unit effect parameters.

`RenderOrbitingEmitter` describes the observed 8-byte state (phase and
particle-list pointer), with target size/offset assertions. Shared helper
prototypes retain their existing signatures. The historical function symbol
remains; the filename does not claim an original TU boundary. This is game
code, not Psy-Q. The neighboring continuous emitter `func_800D8388` remains
ASM: its candidate still differs in two register-bearing instructions and
is not counted as decompiled.

Matching debt is four GTE pins (`$8`, `$12`, `$13`, `$14`) and one empty
pointer barrier, included in the ratchet. Two volatile halfword accesses
preserve the parameter00 stores before texture lookups; these are additional
source-shape debt documented here, not hardware volatility claims. GTE uses
eight existing macros, each containing one control-register transfer. No
ordinary CPU instruction ASM, NOPs, EABI or toolchain modifications are used.
One integer local is reused for initialization size and the emission loop;
separate locals select the wrong saved register in three instructions.

The production algorithm passes 1866 ASan/UBSan host cases covering seeds,
ages -1 through 45, all eight allocation-success masks, signed coordinate
boundaries, all pairs of eight texture indices, and inactive modes. Mocks
check pool dimensions, callbacks, initialized and untouched fields, list
publication after the update, both upload parameter sets, and GTE transfer
order and values. The host copy removes the empty barrier; its prelude
suppresses pins and unrelated target assertions and records individual GTE
transfers. This tests CPU behavior and transfers, not GTE hardware emulation.
Target compilation and the linked comparison verify the PSX layout and
all 748 bytes. Scratch proof is in `/tmp/pe-orbiting-burst/`.

`make verify-clean` passes all 340 tests and source, organization and updated
debt gates. The complete main image and all 191 rebuilt overlays preserve
their retail SHA-1 values.


### Vertical field effect and emitter reconstruction

`func_800D8978` and `func_800D8B6C` share `FieldEng_VerticalEffect.c` and
match 500 and 424 retail bytes respectively (924 bytes together), using
stock GCC 2.7.2 and unchanged MASPSX. The callback follows the shared X/Z
center and computes Y as center Y minus age times 900 divided by 24. It
advances the halfword angle/timer and completes at age 24. Drawing starts
with a sine-controlled scale, switches to a fixed scale at timer 8, samples
the color track, selects a palette, and draws texture 68.

The emitter allocates fourteen 12-byte payloads, emits on odd ages below 32,
initializes stage/timer/angle/Y, and completes at age 50. Its update mode
intentionally falls through into the center/parameter refresh; drawing mode
performs only that refresh. The 12-byte `RenderVerticalEffect` layout follows
the accesses and the allocator stride, with size/offset assertions. The
center declaration is a view into existing storage, not a new definition.
Both original symbols remain. Grouping these neighboring, cooperating
functions makes no claim about the original TU boundary. This is game code,
not Psy-Q.

Matching debt is one `$3` pin for the palette comparison constant, included
in the ratchet. The emitter needs no pins. There are no added barriers, NOPs,
volatile accesses, GTE instruction macros, ordinary CPU instruction ASM,
stack padding, EABI or toolchain changes. Ordering the angle update before
the timer update recovers the retail scheduling without extra constraints.

The production source passes 6078 ASan/UBSan host cases covering vertical
motion and signed-coordinate narrowing, angle/timer wrapping, all scale
stages around timer 8, five signed sine results, eight palettes and both
asset-flag states, color/draw arguments, emitter allocation failure/success,
odd/even ages across both cutoffs, position capture, center refresh and
render parameters. The host prelude suppresses the pin and unrelated target
assertions, then checks the new state size explicitly. The linked target
comparison verifies all 924 bytes. Scratch proof is in
`/tmp/pe-rising-effect/`.

`make verify-clean` passes all 340 tests and the source, organization and
updated-debt gates. The complete main image and all 191 rebuilt overlays
preserve their retail SHA-1 values.


### Helical field effect and contracting particle reconstruction

`func_800D8D14` and `func_800D8E74` share `FieldEng_HelicalEffect.c` and
match all 352 and 1032 retail bytes respectively (1384 together), with stock
GCC 2.7.2 and unchanged MASPSX. The small particle moves Y by -3 per update,
completes at age 12, and draws texture 138 with linearly decreasing scale,
a sampled color track and the palette selected at CLUT X=112.

The helical effect orbits the shared center, advances its angle by 96,
changes radius along a 36-tick cosine curve, and moves vertically in one of
two directions. It emits jittered particles on even ages and completes at
age 36. Rendering preserves the three scale stages, radius expansion in the
last stage, constant color and texture 228. The stage-zero `rsin` call is
retained even though retail discards its result; the source does not assume
that call can be deleted.

`RenderHelicalEffect` is a 16-byte state with stage, angle, timer, XYZ,
radius and direction. Size/offset assertions accompany the recovered fields.
The neighboring controller `func_800D927C` requests 16-byte effects and
8-byte particles and initializes direction at offset 0xE. The particle uses
the existing `GteShortVector` and leaves its final halfword untouched. Both
historical symbols remain; grouping the collaborating callbacks does not
claim an original TU boundary. This is game code, not Psy-Q.

Matching debt is one `$3` palette-comparison pin in the helical callback,
included in the ratchet; the particle needs no pins. A signed-halfword cast
on one vertical offset keeps the retail evaluation order before adding the
center. This has the same final halfword result over the effect's input
range. No barriers, NOPs, volatile accesses, instruction ASM, stack padding,
EABI or compiler/assembler modifications are introduced.

The production source passes 20173 ASan/UBSan host cases covering ages around
both completion cutoffs, both vertical directions and zero/nonzero selector
values, signed trig results, allocation failure/success, all jitter masks,
coordinate/angle/timer narrowing, every scale stage and transition, eight
palettes and both asset-flag states, sampled/constant color, and all draw
arguments. Mocks also verify the otherwise unused sine call. The host
prelude suppresses the pin and unrelated target assertions, then checks both
state sizes explicitly. Linked target comparison verifies all 1384 bytes;
scratch source and proof are in `/tmp/pe-helical-effect/`.

`make verify-clean` passes all 340 tests and the source, organization and
updated-debt gates. The complete main image and all 191 rebuilt overlays
preserve their retail SHA-1 values.


### Helical emitter reconstruction

`func_800D927C` now joins its two callbacks in `FieldEng_HelicalEffect.c`.
The emitter matches all 728 retail bytes, and the combined three-function
unit matches all 2112 bytes with stock GCC 2.7.2 and unchanged MASPSX.
Initialization seeds the phase and allocates four 16-byte helical effects
plus thirty-two 8-byte particles. At ages below 2 it attempts two allocations,
initializes their angles half a revolution apart and direction from the age,
and advances the emitter phase by 1024 even if allocations fail. It updates
and publishes the particle list until age 70. Drawing transfers the matrix
to GTE, renders particles with the 16-unit parameters, refreshes the shared
center and installs the 32-unit effect parameters.

The emitter reuses `RenderOrbitingEmitter`: the same observed 8-byte layout
of phase and particle-list pointer, already protected by size/offset asserts.
The original symbol remains and the adjacent ASM boundary moves to
`0x800D9554`. This groups the controller and its callbacks without claiming
an original TU boundary. This is game code, not Psy-Q.

Additional matching debt is four GTE register pins (`$8`, `$12`, `$13`, `$14`)
and one empty pointer barrier, included in the ratchet. Two volatile halfword
stores preserve parameter00 ordering before texture lookups; these are
source-shape debt, not claims that the parameter block is hardware volatile.
The eight GTE transfers each use an existing single-instruction macro. There
are no ordinary CPU instruction ASM, NOPs, EABI, compiler or assembler
modifications. Capturing the phase before initializing effect fields recovers
the retail load ordering without another constraint.

The production emitter body passes 658 ASan/UBSan host cases covering seeds,
ages -1 through 72, every two-allocation success mask, angle narrowing,
direction values, untouched XYZ fields, list publication after update,
all pairs of eight texture indices, center capture, both parameter uploads,
GTE transfer order/values, and inactive modes. The host harness extracts the
emitter body, removes its empty barrier, suppresses pins/target assertions,
and mocks each GTE transfer individually; it does not emulate GTE hardware.
The linked combined-unit comparison verifies the layout and all 2112 bytes,
including the previously matched callbacks. Scratch proof is in
`/tmp/pe-helical-emitter/`.

`make verify-clean` passes all 340 tests and the source, organization and
updated-debt gates. The complete main image and all 191 rebuilt overlays
preserve their retail SHA-1 values.


### Damped spark and emitter reconstruction

`func_800D9554` and `func_800D96F4` share `FieldEng_DampedSpark.c` and match
all 416 and 920 retail bytes respectively (1336 together), with stock GCC
2.7.2 and unchanged MASPSX. The particle updates XYZ from its velocity,
adds random X/Y jitter, damps X/Z velocity by 31/32, reverses vertical
acceleration at age 19, and completes at age 40. Drawing samples a color
track before age 19, then randomly selects packed color 0xC8C8 or zero.

The emitter captures the actor position and seeds a phase, allocates 24
12-byte particles, attempts two emissions per update before age 12, and
completes at age 70. Successful allocations receive speed 43–50, horizontal
velocity from sine/cosine, vertical velocity -10–21 and a phase step of
0x955. Rendering uploads the 32-unit parameters, draws texture 66 with
animated scale/intensity before age 33, then restores the GTE matrix and
depth parameter on every rendering call.

`RenderDampedSpark` describes six signed halfwords (XYZ and velocity), and
`RenderSparkEmitter` describes an 8-byte position followed by a 32-bit phase.
Both 12-byte layouts have size/offset assertions; particle stride is also
confirmed by the emitter allocation. The color remains a packed 32-bit
local where retail writes full words. Helper/global declarations live in
the shared header. Symbols remain unchanged; grouping this pair makes no
original-TU claim. This is game code, not Psy-Q.

The particle needs no matching constraints. Emitter debt is five pins (four
GTE transfer registers and the palette comparison constant) and two empty
barriers (matrix pointer and completed scale), included in the ratchet. The
scale barrier keeps its addition ahead of palette selection; without it,
the pinned comparison changes scheduling. Eight existing single-instruction
GTE macros express the transfers. There are no added NOPs, volatile accesses,
ordinary CPU instruction ASM, stack padding, EABI or toolchain changes.

The production algorithm passes 77181 ASan/UBSan host cases: velocity and
coordinate extremes, damping of both signs, acceleration/lifetime cutoffs,
random jitter and color choices, allocation success masks, signed trig
results, phase steps, all texture/palette indices and both asset-flag states,
render cutoff and matrix/depth restoration. A mocked upload changes the
palette selector to exercise every branch after that call. The host copy
removes the two empty barriers; its prelude suppresses pins and unrelated
target assertions and records each GTE transfer. This verifies CPU behavior
and transfers, not GTE hardware emulation. Target assertions and linked
comparison verify both layouts and all 1336 bytes. Scratch proof is in
`/tmp/pe-damped-spark-emitter/`.

`make verify-clean` passes all 340 tests and the source, organization and
updated-debt gates. The complete main image and all 191 rebuilt overlays
preserve their retail SHA-1 values.


### Ballistic sprite and emitter reconstruction

`func_800D9E5C` and `func_800D9FD4` share `FieldEng_BallisticSprite.c` and
match all 376 and 552 retail bytes respectively (928 together), using stock
GCC 2.7.2 and unchanged MASPSX. The particle moves Y by its velocity, decreases
that velocity by 2 before age 19, and completes at age 24. Drawing rotates
around Y and draws texture 136 with fixed scale, sine-based intensity,
CLUT X=96 and a null color pointer.

The emitter captures the actor position, allocates twelve 8-byte particles,
and attempts an emission on odd ages below 32. Successful allocations copy
Y, jitter X/Z with signed `rand() % 400 - 200`, and choose initial vertical
velocity -16 through -23. It completes at age 70. Drawing installs the
32-unit render parameters and uploads the selected texture.

The particle reuses the existing 8-byte `RenderArcingEffect` (XYZ and
vertical velocity), whose assertions and the emitter's allocation stride
agree. The emitter uses the existing short-vector view and does not modify
its padding halfword. Original symbols remain; grouping this controller
and callback makes no original-TU claim. This is game code, not Psy-Q.
There are no new pins, barriers, NOPs, volatile accesses, instruction ASM,
stack padding, EABI, compiler/assembler changes or other ratcheted debt.

The production source passes 81487 ASan/UBSan host cases covering coordinate
and velocity extremes, acceleration/lifetime cutoffs, signed sine outputs,
every palette and both asset-flag states, allocation failure/success,
negative and boundary random values for signed modulo, all velocity masks,
position capture, parameter upload, unchanged fields and inactive modes.
Mocks check callback identity and every draw argument, including the null
color pointer. Only unrelated target assertions are suppressed for the host;
the reused particle size is checked explicitly. Linked target comparison
verifies all 928 bytes. Scratch proof is in `/tmp/pe-ballistic-sprite/`.

`make verify-clean` passes all 340 tests and the source, organization and
debt gates without increasing the baseline. Main and all 191 rebuilt overlays
preserve their retail SHA-1 values.

### Attached point emitter reconstruction

`func_800DA5D4` in `FieldEng_AttachedPointEmitter.c` matches all 428 retail
bytes with stock GCC 2.7.2 and unchanged MASPSX. It seeds the emitter angle,
allocates 24 payloads of 16 bytes, emits every sixth age below 40, and ends
at age 70. Allocation failure leaves the angle unchanged. A successful
emission initializes the existing point prefix and subtracts 1365 plus the
low random byte from the emitter angle.

Drawing copies the active owner's actor target coordinates and overrides Y
with `D_800942EC`. The observed owner prefix has an actor pointer at +8;
`FieldActor.render_object.target_x/y/z` supplies the existing +0x268/26A/26C
layout rather than introducing a duplicate actor structure. No meaning is
claimed for the owner's first eight bytes or the height global beyond these
accesses. This is game code, not Psy-Q; no original TU boundary is asserted.

There are no new pins, barriers, volatile accesses, instruction ASM or other
ratcheted debt. The production C passes 90368 ASan/UBSan cases checking all
ages -12 through 75, both allocation outcomes, 512 random values, payload
fields left untouched, initialization callback/stride/count, signed position
extremes and inactive modes. Linked comparison verifies all 428 bytes;
scratch proof is in `/tmp/pe-attached-point/`.

`make verify-clean` passes all 340 tests and source/organization/debt gates.
The rebuilt main retains SHA-1 `452fb033f2eaa4b18aa20a5bca60b8125af3a37b`.
All 191 rebuilt overlays also retain their retail SHA-1 values.

### Rotating ballistic sprite reconstruction

`func_800DA780` in `FieldEng_RotatingBallisticSprite.c` matches all 436
retail bytes with stock GCC 2.7.2 and unchanged MASPSX. It reuses the existing
8-byte `RenderArcingEffect` payload. Updates add vertical velocity to Y,
decrease velocity by one before age 19, and finish at age 24. Drawing copies
XYZ, uses Z rotation X + Y + age * 32, sine intensity divided by 128, fixed
scale 4096 and texture 96 + signed parameter02 * (age / 6). The signed cast
is supported by the original `lh` at 0x800DA8D8. The palette adjustment,
CLUT X=96 and null color argument follow the retail call sequence.

There are no new pins, barriers, volatile accesses, instruction ASM or other
ratcheted debt. This is game code, not Psy-Q, and the filename makes no claim
about the original TU. The production source passes 37154 ASan/UBSan cases:
short-coordinate/velocity extremes, age cutoffs, signed sine division around
128, all palettes and asset flag states, signed texture parameters including
-32768/32767, unchanged draw state and inactive modes. Mock draw calls check
all arguments. The host explicitly checks the 8-byte payload size; unrelated
target layout assertions are suppressed. Scratch proof and the exact linked
byte comparison are in `/tmp/pe-rotating-ballistic/`.

`make verify-clean` passes all 340 tests and the source, organization and
debt gates. Main retains SHA-1 `452fb033f2eaa4b18aa20a5bca60b8125af3a37b`.
All 191 rebuilt overlays preserve their retail SHA-1 values as well.

### Rotating ballistic sprite emitter reconstruction

`func_800DA934` now shares `FieldEng_RotatingBallisticSprite.c` with its
callback `func_800DA780`. The emitter matches 612 retail bytes; the complete
unit matches 1048 bytes using stock GCC 2.7.2 and unchanged MASPSX.
Initialization captures the active owner's actor target XYZ, replaces Y with
`D_800942EC`, and allocates twelve eight-byte particles. The existing owner
prefix and `FieldActor` target fields supply the offsets; no duplicate
structure or original-TU claim is introduced.

Updates attempt emission on odd ages below 40 and complete at age 70.
Successful allocations jitter X/Z by signed `rand() % 400 - 200`, lower Y
by 800, and choose upward velocity 42 through 49. Drawing installs 32-unit
render parameters, uploads the texture and sets depth to 32.

The emitter adds one register pin for the stride argument (`$5`) and one
empty memory barrier. The pin prepares the argument before the position
loads; the barrier consumes the task-slot pointer and preserves the retail
ordering of target-Z storage and the height override. Three exploratory
pointer pins were removed. There are no CPU instruction bodies, volatile
accesses, NOPs, EABI or compiler/assembler changes. These two constraints
are counted in the debt baseline under the user's permission for documented
pins and empty barriers.

The combined production unit passes 134622 ASan/UBSan cases, including the
callback's existing checks, position capture across signed-short extremes,
allocation failure/success, emission and lifetime boundaries, signed random
modulo, all velocity masks, texture upload parameters, unchanged state and
inactive modes. Mocks verify allocation stride, count and callback identity.
Host compilation removes the target register annotation/empty barrier and
unrelated target layout assertions; the particle's eight-byte size is checked.
The linked target check verifies all 1048 bytes. Proof is under
`/tmp/pe-rotating-emitter/` (`production-check.py`, `test.c`).

`make verify-clean` passes all 340 tests and source/organization/debt gates.
Main retains SHA-1 `452fb033f2eaa4b18aa20a5bca60b8125af3a37b`.
All 191 rebuilt overlays retain their retail SHA-1 values.

### Cosine particle emitter reconstruction

`func_800DACA4` now shares `FieldEng_CosineEffect.c` with its callback.
The emitter matches all 744 retail bytes; the combined unit matches all 1012
bytes with stock GCC 2.7.2 and unchanged MASPSX. Existing `RenderSparkEmitter`
(position and phase), `RenderCosineEffect` (8-byte particle) and `RenderColor`
types cover its accesses. Shared declarations replace candidate-local externs.
The render helper's rotation argument is supported by its call to RotMatrixYXZ
and read of rotation flags at +6; its two color arguments are read as RGB bytes.
No original TU claim is made, and this is game code rather than Psy-Q.

Initialization stores a random phase, captures actor position in mode zero
and allocates 24 eight-byte particles. Updates emit before age 51, choosing
amplitude 500..755 and duration 22..25, copying phase into particle Y and
advancing it by 2218..2249. The particle X is left untouched. The emitter
finishes at age 80. Drawing through age 80 inclusive sets depth 60 and draws
three layers with shared sine intensity and growing scale; every draw-mode
call then restores depth 64 and updates the translation. The two local color
copies preserve the original unaligned four-byte transfers at function entry.

There are no new pins, barriers, volatile accesses, instruction ASM or other
ratcheted debt. The production unit passes 198592 ASan/UBSan emitter cases:
initialization, allocation failure/success, emission and lifetime cutoffs,
random masks including signed extremes, all phase increments, untouched
fields, draw cutoff/intensity/coordinates, all layer arguments and inactive
modes. A mock changes the second color global during the first draw to verify
that the later draw uses its entry-time local copy. Host checks assert both
payload sizes; unrelated target layout assertions are suppressed. The existing
callback is not exercised by this emitter harness, but the linked comparison
covers the complete 1012-byte unit against the retail executable. Scratch
proof is in `/tmp/pe-cosine-emitter/` (`production-check.py`, `test.c`).

`make verify-clean` passes all 340 tests and source/organization/debt gates
without a baseline change. Main retains SHA-1
`452fb033f2eaa4b18aa20a5bca60b8125af3a37b`.
All 191 rebuilt overlays retain their retail SHA-1 values.

### Projected four-line diamond reconstruction

`func_800DB25C` in `FieldEng_LineDiamond.c` matches all 920 retail bytes
with stock GCC 2.7.2 and unchanged MASPSX. It reserves 64 packet bytes,
projects the input point with GTE, applies screen offsets, computes two
opposite point pairs from sine/cosine at angle and angle+1024, then fills
and queues four flat-color line packets. RGB is multiplied by the signed
intensity and divided by 128; endpoints and colors retain halfword/byte
truncation. The active ordering-table slot is read again for each edge.

`RenderLinePacket` describes the observed 16-byte tag/RGB/code/XY layout.
`RenderBufferPrefix` covers the buffer-pointer prefix initialized by
`Boot_InitMemoryLayout`: two ordering buffers, six other buffer pointers,
and two packet buffers at +0x20. Assertions pin the target layout. This
replaces the candidate's overly broad flat pointer table without adding
symbol aliases. Shared declarations live in `render_prim.h`; no original
TU boundary or Psy-Q provenance is claimed for this game function.

Four GTE instructions each use an existing single-instruction macro:
two vertex loads, RTPS, and SXY2 store. The two retail hazard NOPs use
`PE1_NOP` and add exactly two `nop_barriers` to the documented baseline,
under the user's explicit NOP allowance. There are no new register pins,
empty barriers, volatile data accesses, CPU instruction bodies beyond those
NOPs, EABI, or compiler/assembler changes. Ordinary packet construction and
all arithmetic are C; the edge macro only repeats C statements.

The production source passes 2744 ASan/UBSan cases checking both draw slots,
packet offsets, signed coordinate/radius extremes, signed intensity/color
truncation, positive and negative sine/cosine products, untouched packet
regions, and exact four-edge ordering. A mock flips the active draw slot
between submissions to verify each lookup is fresh. Host GTE hooks check
transfer/NOP/command/store ordering and supply projected screen coordinates;
they do not claim to emulate GTE projection. Target byte comparison verifies
the actual GTE opcodes and scheduling. Host checks assert the 16-byte packet
size; unrelated target assertions are suppressed. Proof is under
`/tmp/pe-line-diamond/` (`check.py`, `test.c`, `host-prelude.h`).

`make verify-clean` passes all 340 tests and source/organization/debt gates.
Main retains SHA-1 `452fb033f2eaa4b18aa20a5bca60b8125af3a37b`.
All 191 rebuilt overlays retain their retail SHA-1 values.

### Jitter sprite and emitter reconstruction

`func_800DBCD8` and `func_800DBE6C` share `FieldEng_JitterSprite.c` and
match 404 and 492 retail bytes respectively (896 together), with stock
GCC 2.7.2 and unchanged MASPSX. Both use the existing eight-byte short-vector
payload and leave its padding halfword untouched. The emitter's position
capture reuses the existing active-owner prefix and actor target fields.
There are no new structures, pins, barriers, volatile accesses, NOPs,
instruction ASM or other ratcheted debt. This is game code, not Psy-Q;
the grouping makes no original TU claim.

The particle jitters X/Z by -3..4, advances Y by 0..3, and completes at
age 32. Drawing uses the color track at D_800E1DA4, Z rotation age*12,
scale 6144+age*128, CLUT X=192 and texture
192 + signed parameter02*(age/8+2), with fixed intensity 128. The emitter
allocates twenty eight-byte particles, emits each update below age 21,
jitters each captured coordinate by -256..255, and completes at age 53.
Its render setup uploads the selected texture with 32-unit extents and
depth 16.

The production unit passes 135226 ASan/UBSan cases covering coordinate
wraparound, every update mask, age cutoffs, color/rotation/scale/texture
arguments, all palettes and both asset flags, signed texture parameters,
actor position capture, allocation failure/success, signed and boundary
random values, preserved padding, render setup and inactive modes. Mocks
check allocation stride/count/callback and every draw argument. The host
explicitly checks the eight-byte payload; unrelated target assertions are
suppressed. The linked byte comparison covers all 896 bytes. Scratch proof
is in `/tmp/pe-jitter-sprite/` (`check.py`, `test.c`).

`make verify-clean` passes all 340 tests and source/organization/debt gates
without a baseline change. Main retains SHA-1
`452fb033f2eaa4b18aa20a5bca60b8125af3a37b`.
All 191 rebuilt overlays retain their retail SHA-1 values.

### Sine-scaled jitter sprite and emitter reconstruction

`func_800DC5BC` and `func_800DC750` share `FieldEng_SineJitterSprite.c`
and match all 404 and 448 retail bytes respectively (852 together), using
stock GCC 2.7.2 and unchanged MASPSX. It reuses the existing
short-vector payload, jitters X/Z by -3..4, decreases Y by 2..5 and
completes at age 14. Drawing uses the D_800E1E64 color track, Z rotation
age*128 and scale rsin(age*2048/14), texture 226 and intensity 128.
CLUT X is zero; the palette adds six in the special asset case and two
otherwise. Payload padding and all state in draw mode remain untouched.
This is game code, not Psy-Q; no original TU claim is made.

The final source adds one pin for the palette comparison constant in `$3`
and one empty input barrier after rsin. Together they preserve the retail
placement of the sine result and the constant/index registers. Exploratory
selector/offset pins and raw pointer indexing were removed; the final lookup
is a normal array access. The two constraints are included in the debt
baseline under the user's permission for documented pins and empty barriers.
There are no volatile accesses, instruction ASM, NOPs, EABI or toolchain changes.

The production unit passes 111542 ASan/UBSan cases covering signed-short
wraparound, all random masks, the lifetime boundary, every palette and both
asset states, positive/zero/negative sine results, color copies, rotation,
scale, every draw argument, unchanged state and inactive modes. The host
checks the eight-byte payload size and suppresses unrelated target assertions
and the pin/empty barrier; the linked target comparison verifies all bytes.
The emitter captures actor position in mode zero, allocates twenty eight-byte
particles, emits at ages below 40 and finishes at age 53. Each coordinate
gets independent -256..255 jitter. Render setup selects D_800E11F6's texture,
sets parameter06 to one, and uses depth 16. It adds no pins or barriers.
Additional tests check capture, allocation failures, signed random values,
all coordinate writes and preserved padding, both lifetime boundaries,
texture setup and inactive modes. The linked comparison covers the full
852-byte unit. Proof is in `/tmp/pe-sine-jitter/` (`check.py`, `test.c`).

`make verify-clean` passes all 340 tests and source/organization/debt gates.
Main retains SHA-1 `452fb033f2eaa4b18aa20a5bca60b8125af3a37b`.
All 191 rebuilt overlays retain their retail SHA-1 values.

### Radial three-layer flash reconstruction

`func_800DBA9C` in `FieldEng_RadialFlash.c` matches all 572 retail bytes
with stock GCC 2.7.2 and unchanged MASPSX. It reuses `RenderSparkEmitter`
(position and phase), `GteShortVector`, and `RenderColor`; no new layout is
introduced. Initialization captures the shared phase, advances it by 1365,
captures actor position in mode zero, lowers Y by 100 and adds the
radius-300 sine/cosine offsets to X/Z. Its direct return preserves the
retail return-value scheduling. Update completes at age eight.

Drawing sets depth 60 and copies position, then renders three layers:
500x500 with growing scale, 410x500 at scale 4096, and 500x100 with growing
scale. The first two use the D_800E1D64 color track; the last uses D_800E1D84.
The common intensity is 128 and growing scale is 2048+age*512. All arithmetic
and control flow are C. There are no new pins, barriers, volatile accesses,
NOPs, instruction ASM or other ratcheted debt. This is game code rather than
Psy-Q, and no original TU boundary is claimed.

The production function passes 1652 ASan/UBSan cases covering phase capture
and advancement, signed-short coordinate wraparound, positive/negative
trigonometric division, initialization call ordering, lifetime cutoffs,
position/color copies, every draw argument and inactive modes. Tests keep
phase advancement in the signed-int domain, including INT_MAX-1365.
The host asserts the 12-byte state and suppresses unrelated target layout
assertions. Linked target comparison verifies every byte. Scratch proof is
in `/tmp/pe-radial-flash/` (`check.py`, `test.c`).

`make verify-clean` passes all 340 tests and source/organization/debt gates
without a baseline change. Main retains SHA-1
`452fb033f2eaa4b18aa20a5bca60b8125af3a37b`.
All 191 rebuilt overlays retain their retail SHA-1 values.

### Drifting particle emitter reconstruction

`func_800DCA80` matches all 588 retail bytes with stock GCC 2.7.2 and
unchanged MASPSX. It joins the contiguous `FieldEng_DriftingEffect` callback
in its existing unit; all 956 bytes of the combined unit match retail.
The relationship follows the emitter's actual callback pointer and shared
eight-byte `GteShortVector` payload, without claiming an original TU boundary.
This is game code, not Psy-Q. No pins, barriers, volatile accesses, NOPs,
instruction ASM or other ratcheted debt are added.

Initialization captures position through `func_800CE870` and allocates twenty
eight-byte particles. Updates emit on odd ages below 33, with independent
X/Z offsets `rand()%800-400` and Y offset `rand()%400-700`, and finish at age
60. Signed remainder behavior is preserved. Rendering configures the selected
texture, 32-unit extents and depth 16. Particle padding remains untouched.

The production unit passes 94792 ASan/UBSan host cases covering captured
coordinates, callback/stride/count, allocation failures, odd/even ages and
lifetime cutoffs, signed random extremes, short-coordinate wraparound,
preserved padding, texture setup and inactive modes. Host checks assert the
eight-byte payload while suppressing unrelated target layout assertions.
The independent linked comparison verifies the entire combined unit against
retail executable bytes. Scratch proof is in `/tmp/pe-drifting-emitter/`
(`check.py`, `test.c`), with the standalone function check also retained.

`make verify-clean` passes all 340 tests and source/organization/debt gates
without a baseline change. Main retains SHA-1
`452fb033f2eaa4b18aa20a5bca60b8125af3a37b`.
All 191 rebuilt overlays retain their retail SHA-1 values.

### Tilting sprite and emitter reconstruction

`func_800DCCCC` (456 bytes) and `func_800DCE94` (776 bytes) form the
contiguous `FieldEng_TiltingSprite.c` unit. Stock GCC 2.7.2 and unchanged
MASPSX reproduce all 1232 bytes, independently compared with both the retail
disassembly and executable range. No pins, barriers, volatile accesses, NOPs,
instruction ASM or other ratcheted debt are added. This is game code, not
Psy-Q, and the grouping does not claim an original TU boundary.

The emitter allocates sixteen 16-byte particles. Its writes and the callback's
reads establish `RenderTiltingSprite`: an eight-byte rotation prefix, height,
width, initial X tilt and Y angular velocity. The draw routine passes that
prefix to `RotMatrixYXZ`. Header assertions cover size and key offsets.
The existing 12-byte `RenderSparkEmitter` holds the emitter position and phase.

The particle advances Y rotation by its velocity and Z by eight, decreases
X tilt by initial_tilt*age/128, sets height 114 and width 200+rcos(age*16)/4,
and completes at age 64. Rendering selects an eight-frame UV sequence,
CLUT X=176 and the D_800E1F18 color track. The emitter captures position,
emits below age 17, advances phase by 2218 plus five random bits, and completes
at age 73. Through age 64 it draws three layers with growing and sine-scaled
sizes. It always updates the shared position and texture setup in draw mode.
Fields not written by the retail emitter or callback remain untouched.

The production unit passes 37020 ASan/UBSan cases covering rotation wraparound,
signed division, initial tilt and angular velocity, lifetime boundaries, UV
frames and palettes, all draw arguments, allocation failure/success, signed
random inputs, phase advancement, preserved fields and inactive modes.
Emitter phase tests stay within the signed-int arithmetic domain. Tests assert
both payload sizes while suppressing unrelated target layout assertions.
Scratch proof is in `/tmp/pe-tilting-sprite/` (`check.py`, `test.c`).

`make verify-clean` and final `make verify` pass all 340 tests and the
source/organization/debt gates without a baseline change. Main retains SHA-1
`452fb033f2eaa4b18aa20a5bca60b8125af3a37b`.
All 191 rebuilt overlays retain their retail SHA-1 values.

### Rotating three-layer flash reconstruction

`func_800DD19C` in `FieldEng_RotatingFlash.c` matches all 484 retail bytes
with stock GCC 2.7.2 and unchanged MASPSX. It reuses `RenderSparkEmitter`,
`GteShortVector`, `GteRotation` and `RenderColor`; no new layout is introduced.
Initialization stores a random phase, captures position with mode one and
lowers Y by 510. Update completes at age 64 without changing the state.
Drawing sets depth 60 and renders three layers: 500x500 with growing scale,
410x500 at scale 4096, and 400x100 with growing scale and Z rotation age*16.
The scale is 4096+age*32; the first two layers use D_800E1FA4's color track
and the third uses D_800E1FCC. State padding and phase remain untouched by
update and draw. This is game code, not Psy-Q, and no original TU is claimed.
There are no new pins, barriers, volatile accesses, NOPs, instruction ASM or
other ratcheted debt.

The production function passes 503 ASan/UBSan cases covering random extremes,
position capture and short-coordinate wraparound, lifetime cutoffs, state
preservation, color reuse and call order, all three draw calls and inactive
modes. Host checks assert the 12-byte payload while suppressing unrelated
target layout assertions. An independent linked comparison verifies every
byte against both retail disassembly and the executable range. Scratch proof
is in `/tmp/pe-rotating-flash/` (`check.py`, `test.c`).

`make verify-clean` passes all 340 tests and source/organization/debt gates
without a baseline change. Main retains SHA-1
`452fb033f2eaa4b18aa20a5bca60b8125af3a37b`.
All 191 rebuilt overlays retain their retail SHA-1 values.

### Converging sprite and emitter reconstruction

`func_800DD380` (1004 bytes) and `func_800DD76C` (632 bytes) form the
contiguous `FieldEng_ConvergingSprite.c` unit. Stock GCC 2.7.2 and unchanged
MASPSX reproduce all 1636 bytes, checked against both retail disassembly and
the executable range. The actual allocation stride and callback pointer
establish the grouping, without claiming an original TU boundary. This is
game code, not Psy-Q. There are no pins, barriers, NOPs, volatile accesses or
instruction ASM, and no ratcheted debt increase.

`RenderConvergingSprite` has an eight-byte position followed by signed
halfwords for stage, timer, burst count and phase. Its emitter holds initial
and target short vectors. Size/offset assertions document these layouts.
The shared `LoadAverageShort12` declaration preserves the implementation's
existing void-pointer interface; it does not retype the GTE implementation.

Update increments the timer and finishes once stage reaches two. Draw stage
zero interpolates toward D_800E2234 over 52 ticks, adds an X sine wobble,
lowers the stored Y by two, evaluates the color track, and transitions to
stage one. Stage one uses a sine scale and packed color, then selects a new
random position around the target every twelve ticks until the fourth burst
finishes the effect. The current draw retains the position captured before
that reset. As in retail, draw mode requires stage zero or one; other stage
values leave its position/color locals undefined. No fallback is invented.

The packed color store deliberately uses the target's little-endian word
view of a four-byte-aligned `RenderColor`. The cast is a compiler/ABI-specific
representation constraint, not a claim of portable strict-aliasing C. The
host harness uses `-fno-strict-aliasing`; the production stock GCC profile is
unchanged. Temporary union/dummy-pointer experiments are not retained.

The emitter captures its starting position and a target 550 units above it,
allocates twenty-four 16-byte particles, emits on odd ages below 49, and
finishes at age 140. Emission preserves untouched payload fields and padding.
Its draw mode updates the shared target and texture parameters.

The production unit passes 37145 ASan/UBSan cases covering signed-short
wraparound, timer/stage boundaries, burst resets, both color paths, signed
random inputs, palette selection, all draw arguments, allocation failure and
success, target setup and inactive modes. The interpolation mock checks the
input pointers and weights and supplies known coordinates; it does not
emulate the GTE. The host asserts both payload sizes and suppresses unrelated
target assertions. Linked target comparison proves the actual calls and all
instruction bytes. Scratch proof is in `/tmp/pe-converging-sprite/`
(`check.py`, `test.c`).

`make verify-clean` passes all 340 tests and source/organization/debt gates
without a baseline change. Main retains SHA-1
`452fb033f2eaa4b18aa20a5bca60b8125af3a37b`.
All 191 rebuilt overlays retain their retail SHA-1 values.

### Settling particle emitter reconstruction

`func_800DDD70` in `FieldEng_SettlingEmitter.c` matches all 824 retail bytes
with stock GCC 2.7.2 and unchanged MASPSX. Initialization captures an eight-byte
position and allocates thirty-two 16-byte particles for `func_800DD9E4`.
`RenderSettlingSprite` names the observed position, stage, timer and phase;
the final halfword remains reserved. Size and offset assertions accompany it.
The callback remains ASM and receives no new progress credit: its separate
candidate still differs in eight division register operands when the GTE
matrix pointer is pinned. No original TU boundary is claimed by this split.

Emission occurs below age 67, offsets X/Z by independent nine-bit jitter and
subtracts signed `rand()%600` from Y, initializes stage/timer, and captures a
random phase. Padding and the reserved halfword remain untouched. Update
completes at age 86. Draw reads the actor's matrix array at entity offset
0x238: offsets 0x274/0x278/0x27C are the translation of matrix 19, using the
existing `RenderMatrix` layout. It truncates these coordinates to signed
halfwords and updates D_800E223C. Below age 87 it renders two layers at scale
2048+age*4096/86, clearing only the local blue component for the second layer.
Texture setup runs even after that draw cutoff; depth remains unchanged then.

This is game code, not Psy-Q. The new function has no pins, barriers, volatile
accesses, NOPs or instruction ASM, and no ratcheted debt increase. Splitting
around the matched emitter leaves two manifest ASM ranges without adding
ASM code or crediting the callback. The audited debt counts stay unchanged.

The production function passes 20733 ASan/UBSan cases covering capture and
allocation parameters, allocation failure, lifetime/emission cutoffs, signed
random extremes, coordinate wrapping, preserved payload fields, matrix-19
translation truncation, both draw calls and color modification, palette/texture
setup, depth preservation and inactive modes. Host tests assert the payload
sizes and suppress unrelated target layout assertions. Independent linking
matches both retail disassembly and executable bytes. Scratch proof is in
`/tmp/pe-settling-emitter/` (`check.py`, `test.c`); the unresolved callback
experiment is under `candidates/settling_sprite/` and `/tmp/pe-settling-sprite/`.

`make verify-clean` passes all 340 tests and source/organization/debt gates
without a baseline change. Main retains SHA-1
`452fb033f2eaa4b18aa20a5bca60b8125af3a37b`.
All 191 rebuilt overlays retain their retail SHA-1 values.

### Diamond emitter and GCC reload reconstruction

`func_800DB6BC` now matches all 992 retail bytes. It joins the contiguous
200-byte `func_800DB5F4` particle callback in `FieldEng_DiamondEmitter.c`;
all 1192 linked bytes match the executable range. The emitter's callback
pointer and twelve-byte allocation stride establish the grouping, without
claiming an original TU boundary. `RenderDiamondEmitter` has an eight-byte
position and five word fields; `RenderDiamondParticle` has six halfwords.
Their sizes and relevant offsets are asserted. The final two particle
halfwords remain untouched by emission; the callback reads color time at +10.
This is game code, not Psy-Q.

The blocker was GCC 2.7.2 reload allocation: using `$8` explicitly for the GTE
matrix pointer excludes it from the spill-register candidates in
`reload1.c:order_regs_for_reload`, moving constant division's multiply-high
reload into `$9`. Stock GCC 2.8.1 and the tested optimization flags did not
match. The compiler and MASPSX remain unchanged.

Both divisions by 24 are now equivalent signed C arithmetic. With arithmetic
right shift and `sign = value >> 31`, compute `high = value/6 + sign`, then
`(high >> 2) - sign`. For nonnegative values this is floor(value/24); for
negative values the sign correction gives truncation toward zero. These
intermediates stay in range for every signed 32-bit input. Inlining allows
GCC to cancel the first sign correction and retain its native multiply-high
sequence. The high word is pinned to `$8`; the two shifted results use `$5`
and `$2`, respectively. No multiply/divide instruction ASM is introduced.

The final debt is eight pins (four in the two division helpers and four in
the GTE transfer block) and one empty pointer barrier, recorded under the
user's authorization for documented pins/barriers. Each of the eight GTE
control-register transfers uses an existing single-instruction macro.
Exploratory clobber barriers and 64-bit multiplication variants are absent.
Consolidation removes three file-local extern declarations and one address
filename; both ratchets were updated from the isolated acceptance tree.

Initialization captures actor position and creates ten particles. Updates
through age 24 compute radius, angle, offsets and size; later updates zero
both offsets, and age 72 completes the emitter. Rendering loads the matrix,
copies the shared position, samples color and draws one diamond. Ages 24..39
add two expanded layers with intensities 64 and 96. The particle callback
retains its existing eight-tick lifetime and fading draw behavior.

The production unit passes 3319327 ASan/UBSan checks, mostly signed-division
comparisons around values spanning the full 32-bit range plus 100000
pseudorandom inputs. They also cover initialization, allocation failure,
particle writes and preserved fields, arithmetic/trig arguments, lifetime
cutoffs, all draw layers, color time, and callback behavior. GTE hooks check
all eight transfer values and their order; pins and the empty barrier are
suppressed on the host. Exact linked bytes prove target register allocation
and GTE opcodes. This is sampled arithmetic testing plus the identity above,
not an exhaustive execution of all 2^32 inputs. Scratch proof is under
`/tmp/pe-diamond-emitter/` (`check-pair.py`, `test.c`, `host-prelude.h`);
compiler investigation and rejected variants are under `/tmp/pe-t0-probe/`.

`make verify-clean` passes all 340 tests and source/organization/debt gates.
Main retains SHA-1 `452fb033f2eaa4b18aa20a5bca60b8125af3a37b`.
All 191 rebuilt overlays retain their retail SHA-1 values.

### Scattered field-particle initialization

`func_800CC2C4` in `FieldAnim_ScatteredParticles.c` matches all 380 retail
bytes with unchanged stock GCC 2.7.2 and MASPSX. It is game field-animation
code. The source uses ordinary C, with no register pins, empty barriers,
volatile scheduling accesses, inline ASM, or compiler flag overrides.

The initializer sets scale to 127 and selects four entries for action ID 3,
otherwise sixteen. Each entry copies the shared XYZ origin and consumes three
random values: X/Z use `(rand() % 11 - 5) * 256`; Y uses
`-(rand() % 16 + 20) * 256`. Multiplication preserves the original fixed-point
scaling without a C left shift of a negative value. Signed remainder behavior
is preserved, including inputs outside the normal nonnegative RNG range.

`FieldAnimScatteredParticles` extends the existing `FieldAnimPointData`
prefix with three parallel 16-element signed-halfword velocity arrays. The
observed offsets are 0x68, 0x88 and 0xA8; the complete written extent is 0xC8.
Compile-time assertions check these boundaries. The six origin bytes at
`D_800E2290` are declared as `FieldAnimPointTriple` in the engine-state header;
older independent scalar declarations elsewhere remain migration debt. No
alias or new file-local extern is introduced. The manifest promotes only this
function's existing range; the filename does not claim a recovered original
translation-unit boundary.

The production source passes 20000 ASan/UBSan host cases. An independent
64-bit arithmetic reference checks signed RNG extremes and pseudorandom
inputs, both entry counts, XYZ bit patterns, random-call order/count, and all
untouched bytes (including inactive entries). Scratch evidence is in
`/tmp/pe-scattered-particles/` (`check.py`, `test.c`).

`make verify-clean` passes all 340 tests and the source, organization and debt
gates. Main retains SHA-1 `452fb033f2eaa4b18aa20a5bca60b8125af3a37b`.
All 191 rebuilt overlays retain their retail SHA-1 values. Debt counts remain
unchanged.

### Burst matrix and parameter setup

`func_800CCBA8` reconstructs 788 retail bytes with unchanged stock GCC 2.7.2
and MASPSX. The function registers script `D_800E0EB8`, initializes three
identity matrices, scales each using its own copied vector, and initializes
four twelve-byte burst parameter blocks. This is game field-animation code.
It adds no pins, barriers, volatile scheduling accesses, inline ASM, NOPs,
or compiler flag overrides. An ordinary inline helper shares the identity
initialization; assignments preserve the MATRIX alignment halfword.

The function joins the immediately following `FieldAnim_BurstEffects.c`
range. Retail table `D_800E0E90` contains this initializer followed by
`func_800CCEBC`, `func_800CCEE8`, `func_800CCF10`, `func_800CCF80` and
`func_800CCF90`, with `func_800CCF98` preceding it in the same descriptor.
This supplies dispatch evidence for the contiguous grouping, rather than
claiming an original object boundary from filenames alone.

`FieldAnimBurstParameters` describes RGB bytes, two parameter bytes, and two
signed halfwords at +8/+10; byte +3 and halfword +6 remain unchanged. Unknown
parameter meanings retain offset names. Size and offset assertions document
the 12-byte layout. Existing GTE matrix/vector types describe the three
matrix globals and three copied constants. Shared declarations live in the
field-animation and engine-state headers. The GTE header now owns the
`Gte_ScaleMatrix` prototype from its existing definition; the rotation-loader
caller uses that declaration instead of its older local void-return prototype.
The shared eight-particle room renderer had the same obsolete prototype; it
also uses the canonical declaration now (rooms m174, m348 and m383). These
scale pointer casts preserve the existing XYZ storage and machine code.

4096 ASan/UBSan host cases exercise the initializer extracted unchanged from
the production TU. Hooks verify the object argument and registration, exact
matrix identity values and untouched alignment bytes, separate local vector
copies (including their padding word), sequential copy/call timing, all
parameter values, and untouched bytes. Hooks also mutate later input vectors
and matrix outputs to check call effects and ordering. This tests setup and
its call contract, not GTE scaling arithmetic itself. Scratch evidence is in
`/tmp/pe-burst-setup/` (`check.py`, `test.c`, `setup-under-test.c`).

The independently linked merged TU matches all 1844 retail bytes, including
the previously reconstructed callbacks (`check-unit.py`). `make verify-clean`
passes all 340 tests and the source, organization and debt gates. Main retains
SHA-1 `452fb033f2eaa4b18aa20a5bca60b8125af3a37b`.
All 191 rebuilt overlays retain their retail SHA-1 values, including the
three users of the corrected room-renderer declaration. Debt counts remain
unchanged.

### Cosine pulse callback

`func_800D5898` in `FieldEng_CosinePulse.c` matches all 360 retail bytes using
unchanged stock GCC 2.7.2 and MASPSX. Mode 1 scales the initial amplitude by
`rcos((age << 10) / duration) / 4096` and ends when age reaches duration.
Mode 2 draws with the stored radial position, the second coordinate/angle,
a local copy of `D_800C22C0`, and cosine-derived scale `/64 + 64`.
The eight-byte state reuses `RenderCosineEffect`; its duration offset +6 is
now asserted. The following emitter allocates 24 entries with stride 8 and
passes this callback, confirming its state size. The draw routine rotates the
first argument around Z by the second; the existing generic x/y member names
are retained. No original TU boundary is inferred from this function split.

There are no pins, barriers, volatile scheduling accesses, inline ASM, or
compiler flag overrides in this source. The existing `--expand-div` MASPSX
option restores the retail divide-by-zero and signed-division-overflow checks
for both variable divisions; MASPSX itself is unchanged. The color struct's
byte alignment also reproduces the original unaligned four-byte copy.

400001 ASan/UBSan host cases cover lifecycle and unsupported modes, all
cosine results from -4096 through 4096 plus repeated sampled values, signed
halfword state values, duration boundaries, and the draw arguments. A 64-bit
reference checks arithmetic and truncation. Tests use nonnegative ages up to
0x1FFFFF and nonzero signed durations, avoiding C division/shift undefined
behavior; real emission initializes duration to 12..15. The draw hook verifies
that color was copied before the trigonometric call by having that call mutate
the source global. This checks the callback contract, not the implementation
of cosine or rendering. Evidence is under `/tmp/pe-cosine-ring/` (`check.py`,
`test.c`). The neighboring emitter remains an unaccepted candidate with three
register-operand differences; it is not included in progress.

`make verify-clean` passes all 340 tests. The final source also passes the
source, organization and debt gates and the rebuilt main checksum:
`452fb033f2eaa4b18aa20a5bca60b8125af3a37b`.
All 191 rebuilt overlays retain their retail SHA-1 values. Debt counts remain
unchanged.

### Expanding flash and bouncing sprite

`func_800D4EA4` in `FieldEng_ExpandingFlash.c` matches all 364 retail bytes.
It completes on update at age 20. Rendering captures actor position, lowers
Y by 300, chooses a color track from context variable 6, and draws an eight-
segment effect with sine-driven scale. Track time uses `age * 24 / 20`;
scale uses `rsin((age << 10) / 20) / 2 + 2048`. The two track declarations
belong to the shared rendering header, not file-local externs.

`func_800D5010` in `FieldEng_BouncingSprite.c` matches all 640 retail bytes.
`RenderBouncingSprite` has XYZ and three velocity halfwords, followed by
duration at +12 and angle at +14; size and offsets are asserted. Updates
advance all coordinates, damp X/Z velocity by signed `31 / 32`, reverse Y
velocity when the updated Y is positive, and add three to Y velocity before
checking lifetime. Rendering computes fading intensity, rotating angle and
sine-driven scale, selects the palette row (including the special row offset),
and draws with an age-dependent texture argument.

The last opcode difference was the signed load before reversing Y velocity.
Writing `velocity_y *= -1` reproduces retail `lh`; unary negation allowed
stock GCC to use `lhu` before the halfword store. Both expressions give the
same stored halfword on the target, but only the former matches its bytes.
No pin, barrier, volatile scheduling access, inline ASM, or compiler override
is needed. The sprite uses the existing MASPSX `--expand-div` option for the
three retail signed-division guard sequences. Compiler and MASPSX sources
remain unchanged.

These adjacent functions are promoted as two explicit source ranges; shared
rendering dependencies alone are not claimed to prove an original object
boundary. Existing surrounding ASM ranges remain outside the semantic-C
progress count.

The final production sources pass 480001 ASan/UBSan host cases: 80000 for the
flash and 400001 for the sprite. Independent 64-bit arithmetic and explicit
16-bit wrapping check movement, damping, bounce, lifetime and rendering.
Coverage includes signed halfword extremes, both palette branches, preserved
state, unsupported modes, and all sampled sine results across -4096..4096.
Call hooks mutate age/parameters to verify the original later global reads.
Tests avoid zero durations and signed overflow in shifts/products; they test
caller behavior, not the implementations of sine, palette lookup or drawing.
Evidence is under `/tmp/pe-expanding-flash/` and `/tmp/pe-bouncing-sprite/`
(`check.py`, `test.c` in each directory).

`make verify-clean` passes all 340 tests and the source, organization and debt
gates. Main retains SHA-1 `452fb033f2eaa4b18aa20a5bca60b8125af3a37b`.
All 191 rebuilt overlays retain their retail SHA-1 values. Debt counts remain
unchanged.

### Battle script entry dispatch

`Battle_StepScriptEntry` matches all 344 retail bytes with native stock GCC
2.7.2 and unmodified MASPSX. Its existing `-G2` small-data layout is reproduced
with the compiler and assembler options; no register pins, barriers, inline
ASM, volatile scheduling accesses or new declarations are needed. The source
uses the existing `BattleInitSlot`, `BattleEntity` and `Combatant` fields.
The manifest promotes the existing function range without claiming a newly
established original object boundary.

The dispatcher clears motion while a queued entry exists, waits until action
mode 4, then selects player-turn, hit, phase, special-action or escape handling
by the signed command halfword. The escape result is explicitly narrowed to a
signed byte, as in retail. The special-action path rereads the entity pointer,
queue index and flags after `Entity_SetActionMode`; caching those values across
the call would change behavior.

197633 ASan/UBSan host cases cover every signed command value, all byte-valued
action modes and queue indices, exhausted queues, escape-result narrowing and
callee mutations of the globals that must be reread. They test dispatch and
state changes, not the called battle operations. Reproduction evidence lives
under `/tmp/pe-battle-script/` (`check.py`, `test.c`).

`make verify-clean` passes all 340 tests and the source, organization and debt
gates. Main retains SHA-1 `452fb033f2eaa4b18aa20a5bca60b8125af3a37b`.
All 191 rebuilt overlays retain their retail SHA-1 values. Debt counts remain
unchanged.

### Draw-mode packet queueing

`func_800CF6F8` in `FieldEng_QueueDrawMode.c` matches all 332 retail bytes
with stock native GCC 2.7.2 and unmodified MASPSX. It uses ordinary C without
pins, barriers, inline ASM, volatile scheduling accesses or custom flags.
The inline `link_packet` helper preserves each DMA tag's high length byte
while replacing its low 24-bit link, including the ordering-table tag.

For modes other than 255, the function allocates eight bytes from the active
packet buffer, sets the draw mode, marks an optional primitive as translucent,
and links the primitive followed by the draw-mode packet. Mode 255 links only
the optional primitive and allocates nothing. Allocation uses the existing
`RenderBufferPrefix.packets` array at +0x20, confirmed by the retail reference
to `D_800B0E58`; no new buffer-layout hypothesis is needed. The shared header
now declares `GetTPage` and the four-argument `SetDrawMode` implementation.
The surrounding ASM range is split at 0xC0044; this promotion does not claim
that the single-function range proves an original object boundary.

231072 ASan/UBSan host cases check tag linking, length-byte preservation,
translucency, both buffer slots, nullable primitives, the mode-255 bypass,
all signed-halfword mode values and additional sampled 32-bit modes. The
GPU-call hooks verify allocation precedes the calls and mutate the active
buffer/cursor, ensuring the function keeps using the already allocated packet.
They test this function's contract rather than the SDK routines. Narrow byte
comparison and host-test evidence are in `/tmp/pe-draw-mode/` (`check.py`,
`test.c`).

Debt review: `main.pointer_integer_casts` increases from 736 to 737 for the
single `(u32)packet` conversion in `link_packet`. This conversion is the
hardware DMA-address encoding itself, not a scheduling workaround. Keeping
it explicit is preferable to hiding the address conversion through a union
or assembly. The reviewed baseline records this target-ABI dependency, as
with the existing SPU DMA address conversions. This also adds one file to the
audit's dirty-file total (2272 to 2273); pins, barriers, NOPs and aliases are
unchanged.

`make verify-clean` passes all 340 tests, the source/manifest contract and
organization/debt checks with the reviewed baseline. Main retains SHA-1
`452fb033f2eaa4b18aa20a5bca60b8125af3a37b`; all 191 rebuilt overlays also
retain their retail SHA-1 values.
