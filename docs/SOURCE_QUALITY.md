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
