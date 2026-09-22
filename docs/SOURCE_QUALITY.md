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

### Matrix-derived look angles and vector helpers

`func_800CE9D4` matches all 276 retail bytes. It selects the 32-byte matrix at
index `index` from the existing owner table at +0x238, loads its rotation and
zero translation into GTE, and transforms a copied direction vector. It then
narrows the three MAC results to signed halfwords and calls
`FieldEng_CalculateLookAngles` using a separately copied origin. Retail data
at `D_800C2258` is `(0, 0, -16384, 0)` and `D_800C2260` is zero. Both vector
declarations now live in the shared rendering header.

The newly closed range lies directly between `FieldEng_TransformMatrixPoint`
and `FieldEng_RotateVector`. All three use the same matrix-word transfers,
zero-translation setup and vector narrowing; the first two also select the
same +0x238 owner table with a 32-byte stride. They are consolidated into
`FieldEng_MatrixVectors.c` in original address order. The whole contiguous
0x800CE8F0..0x800CEB8C range matches all 668 bytes; the two existing functions
retain their C bodies. This is a supported source grouping, not proof of the
original object-file boundary.

The new function needs only the three GTE transfer-register pins `$12`..`$14`.
A subset search removed the provisional matrix/vector address pins while
preserving the match. Two empty barriers retain local-vector address setup
and loads from the zeroed translation fields. Their operands are initialized,
and neither emits CPU instructions. Each GTE operation remains in its own
existing macro, including the two explicit transfer hazard NOPs. No compiler
or MASPSX modification, EABI, instruction block or new ASM-body wrapper is used.
The reviewed main baseline records pins 1015 to 1018 and barriers 892 to 894;
these increases account for the new function rather than hide its constraints.

200000 ASan/UBSan host cases verify selection among eight matrices, all eight
GTE control writes, both vector loads, hazard/command/store order, zero
translation, signed-halfword narrowing and the look-angle call arguments.
Tests use 64-bit fixed-point products plus arbitrary sampled MAC values and
mutate the source globals during transfer to verify both vectors were copied
first. GTE instructions are replaced by trace hooks and only hard-register
annotations are removed for the host; empty barriers remain. These tests
verify the caller's transfer contract, not GTE timing or look-angle arithmetic.
Evidence is under `/tmp/pe-matrix-angles/` (`check-unit.py`, `test.c`, and the
pin/barrier search scripts).

The consolidated layout passes `make verify-clean`; final `make verify` after
separating the three pin declarations for accurate debt counting also passes
all 340 tests and source/organization/debt gates. The final TU remains an
exact 668-byte match. Main retains SHA-1
`452fb033f2eaa4b18aa20a5bca60b8125af3a37b`, and all 191 rebuilt overlays retain
their retail SHA-1 values.

### Rotation-derived offset with GTE restoration

`func_800CFB7C` in `FieldEng_RotationOffset.c` matches all 376 retail bytes
with native stock GCC 2.7.2 and unmodified MASPSX. It copies the vector at
`D_800C2260`, saves the current matrix pointer, zeros the caller's Z angle,
and builds a local YXZ rotation matrix from the three angle halfwords. GTE
uses that rotation with zero translation to transform the copied vector whose
Z component is replaced by the requested distance. The function restores all
eight GTE control words from the saved matrix pointer before narrowing MAC
results into the output vector. The saved pointer is captured before the call,
but its matrix contents are read when restoring; it is not a value snapshot.

The shared GTE header now gives `RotMatrixYXZ` its implemented return and
argument types: `GteMatrix *(GteShortVector *, GteMatrix *)`. Four existing
main callers drop conflicting file-local void declarations and explicitly
convert their compatible rotation/matrix views. The exact main executable
check verifies those caller updates introduce no instruction changes.

Subset searches reduced the initial nine pins to six and four empty barriers
to three. The retained pins are `$5` for argument staging, `$16` and `$19` for
local/saved matrices, and `$12`..`$14` for GTE transfers. Empty barriers retain
argument lifetimes, delay local matrix register setup, and preserve matrix-
relative loads. All inputs are initialized; the `$16` clobber precedes that
variable's initialization. The reviewed main debt baseline increases pins
1018 to 1024 and barriers 894 to 897. GTE operations and the two transfer
hazard NOPs use existing individual macros, with no CPU instruction block,
new toolchain flag or modified compiler/assembler.

181074 ASan/UBSan host cases cover every signed-halfword distance, sampled
32-bit distances including INT32_MIN/MAX, vector aliasing, signed narrowing,
and exact GTE operation order. Hooks verify the angle Z mutation precedes
`RotMatrixYXZ`, the input vector was copied beforehand, zero translation is
loaded, the original matrix pointer is used even if the call changes the
global slot, and late changes to the original matrix are restored. They also
check that computed-result stores follow the final restore; the initial Z
mutation remains visible when input and output alias. Host adaptation removes
hard-register annotations and the architecture-specific clobber, retains
empty barriers, and replaces GTE instructions with trace hooks. These tests
check the caller contract, not trigonometry or GTE timing. Evidence lives in
`/tmp/pe-rotation-offset/` (`check.py`, `test.c`, pin/barrier search scripts).

`make verify-clean` passes all 340 tests and source, organization and debt
gates. Main retains SHA-1 `452fb033f2eaa4b18aa20a5bca60b8125af3a37b`.
All 191 rebuilt overlays retain their retail SHA-1 values.

### Effect-owner task rendering

`func_800D4704` matches all 332 retail bytes. It establishes the current owner
and task context, optionally looks up the asset-type flag, and traverses all
eight task slots. Each non-0xFFFF slot reloads the current matrix into GTE,
publishes its slot and unsigned halfword age, calls the selected callback in
mode 2, and then renders its non-null child list. Callback return values are
ignored. The child-list pointer is reread after the callback, and the table,
argument and current matrix are looked up again for later active slots.

`FieldAnimTaskOwner` gives this access pattern a typed view using the existing
owner prefix followed by `FieldAnimTaskContext`. Assertions cover context
+0x0C, flags +0x19, slots +0x2C, and table +0x8C. The previously opaque prefix
now names the asset-type selector at byte 1 without changing actor offset +8.
This describes the observed embedding, not a newly inferred allocation size.
The asset lookup prototype belongs to the shared asset-archive header.

The function is added to the adjacent `FieldEng_ObjectLifecycle.c` TU.
`func_800D4620` initializes these same slots and arena; `func_800D4698` invokes
the lifecycle callback using the same context and table offsets. Their bodies
remain unchanged. Together the contiguous 0x800D4620..0x800D4850 range matches
all 560 bytes. The grouping follows shared ownership and lifecycle operations;
it does not claim to establish the original object-file boundary.

Stock native GCC 2.7.2 and unmodified MASPSX produce the match without custom
flags. Subset searches removed the provisional context, loop-index and slot-
address pins. The four retained pins are the matrix pointer `$7` and GTE
transfer registers `$12`..`$14`. One empty barrier keeps loop initialization
and matrix-slot setup after optional asset lookup; its `$20` clobber precedes
matrix-slot initialization. All GTE writes are individual existing macros.
The reviewed main baseline records pins 1024 to 1028 and barriers 897 to 898.
No new instruction ASM, NOP, alias or raw-offset access is introduced.

131072 ASan/UBSan host cases cover all 256 active-slot masks, all byte flag
values, both child-list paths and lookup returns -1/0/1. Hooks change callback
tables, arguments, current matrix pointers and the current slot's child list
to check the retail reload points. Tests verify the fixed eight-slot traversal
with the context count set to zero, unsigned ages, skipped inactive slots,
publication of globals and all eight GTE writes per active slot. The host
adapter extracts the new function from the production TU, removes hard-
register annotations/clobbers and replaces GTE writes with trace hooks. It
tests dispatch rather than callback internals or GTE timing. Evidence is in
`/tmp/pe-draw-tasks/` (`check-unit.py`, `test.c`, `minimize.py`).

`make verify-clean` passes all 340 tests and source, organization and debt
gates. Main retains SHA-1 `452fb033f2eaa4b18aa20a5bca60b8125af3a37b`.
All 191 rebuilt overlays retain their retail SHA-1 values.

### Notification dialog creation

`Menu_CreateNotificationDialog` matches all 348 retail bytes using stock native
GCC 2.7.2 and unmodified MASPSX, without flags, pins, barriers or instruction
ASM. It selects the normal or alternate notification widget, initializes the
parent and child callbacks, copies a 0xFF-terminated message and optionally
appends a second message, then sizes and centers the parent. Widths below 100
use 100; the other path deliberately calls the measurement routine again,
matching retail's call order rather than assuming the result is immutable.
The alternate-buffer selector is a `short` whose value is always zero or one.
This source shape yields the retail register allocation; it does not establish
the original declaration's type.

The function joins the four contiguous notification helpers previously in
`menu16.c`, now `Menu_NotificationDialogs.c`. The entire 980-byte range
0x8004CC50..0x8004D024 matches, including the unchanged earlier routines.
The two 64-byte notification buffers and widget fields use typed accesses;
shared declarations live in subsystem headers. This grouping follows dialog
ownership and shared storage, not a claim about original object boundaries.
The new function adds no matching constraints. The two pre-existing empty
barriers in the two-line helper remain. The reviewed debt change removes two
file-local externs and records two additional accesses to the existing opaque
`field_30` callback member (252 to 254); it adds no raw offsets or aliases.
The organization baseline drops one placeholder filename (69 to 68).

23058 ASan/UBSan host cases exercise both buffers, absent/present suffixes
(including a negative ID passed through the unsigned lookup interface), widths
-256..1024 and changing results on the second measurement. Hooks verify call
order, callback installation, text destination, untouched alternate storage,
width and centering, preserved vertical positions, and queue initialization.
The host fixture extracts the new production function without code changes;
only target-layout assertions are disabled for host pointer sizes. Evidence
is in `/tmp/pe-notification/` (`check-unit.py`, `test.c`, source-shape trials).

The clean main rebuild retains retail SHA-1
`452fb033f2eaa4b18aa20a5bca60b8125af3a37b`. After updating the source-quality
test's renamed TU path, `make verify` passes all 340 tests and the source,
organization and debt gates.
All 191 rebuilt overlays retain their retail SHA-1 values.

### Item-use selection and equipment rollback pointer

`Menu_OpenItemUsePanelAtIndex` matches all 344 retail bytes with native stock
GCC 2.7.2, unmodified MASPSX and the existing `-G8` small-data mode. It allocates
a command, writes opcode 2, resolves the previously tracked equipment slot,
and stores its item-data pointer for rollback. It then reads the selected
filtered-list slot twice, narrows one copy into the tracked selection byte,
and passes the full-width local copy to `Inv_SetActiveList(2, &slot)`.
Out-of-range selections yield zero, and an invalid previous slot yields null.
The inherited function name is retained; these are the observed operations,
not a new claim about the surrounding UI flow.

`InventoryRuntime` describes the contiguous view used by retail's address
calculation: tracked selections at 0x800C0E20, 50 halfword item slots at +0x28,
and 128 existing `ItemDataRecord` equipment records at +0x8C. Offset assertions
check those boundaries. Uninterpreted bytes remain reserved. Equipment lookup
uses the real array and an ID minus 0x100, rather than an invented pre-array
pointer or arithmetic on the tracked-selection pointer. This records verified
adjacency, without claiming an original source-level structure declaration.

The opcode 2/3 `equip_restore.item_data` field is now an `ItemDataRecord *`.
Its rollback consumer and the `Inv_FindIndexByData` declaration use pointer
types too; the PSX layout stays 0x24 bytes. The full main checksum verifies
that the corrected consumer and users of the inventory header retain their
retail bytes. Both command consumers now use the shared inventory view instead
of their scalar/array declarations of the same symbol. Three file-local
externs are removed (3688 to 3685). New cross-unit declarations are in
subsystem headers.

Two retained pins in the inlined lookup select `$5` for the saved item ID and
`$3` for the key-item byte offset. A subset check of these constraints finds
that both are needed in this source shape. Rewriting the filtered-list helper
to merge into a local result removes the provisional empty barrier entirely.
The reviewed main baseline increases pins 1028 to 1030; barriers stay at 898.
There is no new CPU instruction ASM, NOP, alias, pointer/integer cast or raw
field-offset access. The bounded key-item branch retains the established
biased table symbol and byte indexing used by neighboring lookup functions.

396288 ASan/UBSan host cases cover all 65536 item IDs, every signed-byte tracked
index, valid and invalid list bounds, equipment/base/key-item lookup ranges,
signed-halfword selections and byte narrowing. Hooks change the tracked slot
inside command allocation and change the filtered-list bound inside base-item
lookup, verifying the retail reload order. The final hook checks the rollback
pointer, command opcode, full-width local slot and already-updated tracked
byte. Host adaptation removes only register annotations and target layout
assertions. Evidence is in `/tmp/pe-item-use/` (`check.py`, `test.c`, candidates).

The clean main rebuild and final `make verify` retain retail SHA-1
`452fb033f2eaa4b18aa20a5bca60b8125af3a37b`; all 340 tests and source,
organization and debt gates pass. All 191 rebuilt overlays retain their
retail SHA-1 values.

### Inventory selectability predicate

`Inv_IsSlotSelectable` matches all 396 retail bytes. An unresolved item record
returns 1, including an invalid list index; this surprising behavior is
preserved. For a resolved record, flag 0x40 disables selection. The tracked
weapon slot is also excluded when the current list is Aya's inventory. For
kind 8, the routine counts all items of that kind and additionally requires at
least two. It still performs this count when the earlier checks disabled the
selection. The loop reloads the active-list bound after each type-query call.

The inlined lookup uses the existing typed inventory layout and equipment
array. The function joins the adjacent `Item_SetDisabledFlag` in
`Inv_Selectability.c`; both operate on the same availability flag. The setter
body is retained, and all 480 bytes at 0x80057654..0x80057834 match. This is a
semantic grouping, not proof of an original object-file boundary.

The shared type-query prototype returns `int`. Its prior reconstructed
implementation declared `u8`, whereas callers used an integer result. A narrow
return declaration introduces a caller-side mask absent in retail. Correcting
the implementation declaration to `int` preserves all 192 bytes of
`Inv_GetActiveListItemType`, including the unsigned byte field load. Thus the
shared interface and implementation agree without changing either function's
retail behavior; this does not identify the original source declaration.

Stock native GCC 2.7.2 and unmodified MASPSX use the existing `-G8` mode.
Constraint subset checks remove both provisional lookup pins. One empty
barrier separates the valid-index result merge from the invalid-index exit;
removing it merges the branches and loses the retail layout. An explicit
local kind value of 8 before counter initialization reproduces the loop's
constant scheduling without a pin or extra barrier. The reviewed main debt
baseline raises barriers 898 to 899; pins remain 1030. No CPU instruction ASM,
NOP, alias, pointer/integer cast or raw field-offset access is added.

526080 ASan/UBSan host cases cover every 16-bit item ID, every flag and kind
byte, both list identities, equipped versus other slots, signed-byte index
boundaries, and zero/one/multiple kind-8 matches. Hooks switch list identity
inside base-data lookup, shrink or grow the iteration bound during type
queries, and mutate the original record after its flags/kind were read.
Assertions verify the retained selection decision, callback order/count,
null-record result and absence of counting for other kinds. The fixture uses
the production function and helper unchanged, disabling only target-layout
assertions. Evidence is in `/tmp/pe-selectable/` (`check-unit.py`, `test.c`,
`type-int.bin` and candidate subset trials).

The clean main rebuild and final `make verify` retain retail SHA-1
`452fb033f2eaa4b18aa20a5bca60b8125af3a37b`; all 340 tests and source,
organization and debt gates pass.
All 191 rebuilt overlays retain their retail SHA-1 values.

### Parasite resource-cost calculation

`Inv_GetSlotHighlightState` matches all 412 retail bytes with stock native
GCC 2.7.2, unmodified MASPSX and the existing `-G8` small-data mode. The inherited
name is retained, but the reconstructed operations calculate a resource cost:
IDs 6 and 19 return the supplied availability; ID 5 obtains the output value
from `BattleCmd_GetRemainingAmmo` and divides it by 3, ignoring that function's
return value. Other IDs read the table's unsigned halfword cost and inspect
the armor slot at tracked-selection byte 2. Modifier byte 0x0E reduces the
cost to `(cost * 2) / 3`. A missing item record or absent modifier leaves the
cost unchanged. These are observed operations, not a renaming of other
historically labeled ammo/item interfaces.

`ParasiteSpellEntry` describes the four-byte table entries: a halfword unlock
key followed by a halfword cost. The first field and 20-entry bound are also
read by `Aya_UnlockParasiteSpellById`. Compile-time assertions cover size and
cost offset. The archive getter retains its existing `void *` interface.
The new caller decodes it through this typed view. `aya.h` now uses the shared
scalar types from `common.h`; one redundant consumer-local `s16` typedef is
removed. The output-pointer prototype for `BattleCmd_GetRemainingAmmo` is
shared with the existing `Battle_UseItem` caller, replacing its integer-argument
declaration. No existing function body changes are needed.

The lookup uses `InventoryRuntime` and the existing equipment records; stock
GCC naturally reuses the address of tracked byte 2 for equipment addressing.
The modifier scan is an ordinary `for` loop, with no pins. One empty lookup
barrier retains the retail separation between an invalid index and an invalid
item ID; removing it changes the function from 412 to 408 bytes. The reviewed
main baseline increases barriers 899 to 900, with pins unchanged at 1030.
No CPU instruction ASM, NOP, alias, pointer/integer cast or raw field-offset
access is introduced.

364207 ASan/UBSan host cases cover every 16-bit cost with and without the
modifier, every 16-bit item ID, all signed-halfword resource values, 100000
sampled 32-bit values and INT_MIN/MAX division boundaries. They also exercise
all 20 spell IDs, both passthrough cases, null/missing armor, signed-byte slot
boundaries, modifier positions and absence for tail lengths 0..11. Hooks
publish the inventory state inside the table getter and change the table cost
inside base-item lookup, proving the cost was read before that lookup. The
ID-5 hook returns a value different from its output to verify which value is
used. The host fixture uses the production function unchanged and disables
only target-layout assertions. Evidence is in `/tmp/pe-spell-cost/`
(`check.py`, `test.c`, `direct.c` and source-shape trials).

`make verify-clean` passes all 340 tests and the source, organization and debt
gates. Main retains SHA-1 `452fb033f2eaa4b18aa20a5bca60b8125af3a37b`.
All 191 rebuilt overlays retain their retail SHA-1 values.

### Item-action dispatch and explicit argument forwarding

`Inv_BuildEquipSlotDisplay` matches all 416 retail code bytes, and its ordinary
C switch emits all 56 bytes of the jump table at 0x800112A4. The manifest now
assigns that `.rodata` to the C object. The inherited display-oriented name is
retained: the observed routine dispatches item actions. It first resolves the
record, then reads the queued-action mode. In queued mode it allocates an
opcode-0 rollback command, reloads the active list after allocation, saves the
signed item ID and slot, calls `Inv_SetActiveList(0, &item)` and removes the
slot. In immediate mode it dispatches the byte at the start of `bonusStats`:
1 applies/synchronizes the item, removes it and rebuilds the list; 2 opens the
skill view; 4..6 and 12..14 route through menu modes 0xFE and 0x200. Other byte
values do nothing. The unsigned-char view of the existing member is a legal
object-representation access, not an invented field offset. The immediate
path retains retail's valid-record precondition.

The dispatcher joins its three adjacent C functions in `Inv_ItemActions.c`:
selectability, the disabled-flag setter and the resource-cost calculation.
The entire 0x80057654..0x80057B70 range matches all 1308 bytes, plus the switch
table above. Their identical inline lookup is now defined once. The source
debt baseline decreases from 900 to 899 barrier sites because two copies become
one shared site, used by three callers. This is source deduplication, not a
claim that emitted functions need fewer optimizer constraints. There are no
new pins, CPU instruction ASM, NOPs, aliases or pointer/integer casts. Stock
native GCC 2.7.2 and unmodified MASPSX use the existing `-G8` mode.

The dispatch call exposed a semantic omission in the earlier
`BattleCmd_CommitAndSyncAmmo` reconstruction. Retail leaves the item/action ID
in `$a0` for the first call, and `Battle_ApplyDamage` explicitly consumes it.
The C wrapper now accepts `int item` and calls `Battle_ApplyDamage(item)`, with
shared matching prototypes, rather than relying on the register happening to
survive two zero-argument declarations. Three obsolete `M2C_UNK` list-rebuild
declarations are removed from two existing header consumers. The unchanged
wrapper bytes are verified by the full main checksum.

231424 ASan/UBSan dispatcher cases cover all 16-bit item IDs in queued mode,
valid and rejected lookup bounds, and all 256 action bytes for every supported
record ID. Hooks mutate the mode during lookup and the active-list pointer
during command allocation. They verify signed IDs, command payload, the second
item read, exact call order, slot removal, menu arguments and default actions.
A separate 65538-case production-wrapper test verifies explicit forwarding of
every signed-halfword value and INT_MIN/MAX through the null-entity path; it
does not retest the wrapper's pre-existing synchronization internals.
The merged production helper/functions also pass the previous 526080
selectability and 364207 resource-cost host cases. Host adaptation disables
target-layout assertions only. Evidence is in `/tmp/pe-item-dispatch/`
(`check-unit.py`, `test.c`, `test-forward.c`) and the two reused host fixtures.

`make verify-clean` passes all 340 tests and source, organization and debt
gates. Main retains SHA-1 `452fb033f2eaa4b18aa20a5bca60b8125af3a37b`, including
the argument-forwarding wrapper. All 191 rebuilt overlays retain retail SHA-1.

### Slot eligibility excluding equipped weapon and armor

`Inv_IsSlotEquipped` matches all 424 retail bytes at 0x80058E44 using
stock native GCC 2.7.2 and unmodified MASPSX with `-G8`. The inherited
name is retained, but the observed predicate excludes both tracked weapon
and armor slots when the active list is Aya's inventory. Flag 0x20 also
rejects the item. An unresolved record returns 1. Kind 8 additionally
requires at least two entries of that kind in the active list, even when
the flag or equipped-slot check has already rejected the item. The loop
reloads the active-list bound after each type-query call. No stronger
meaning is assigned to flag 0x20 or kind 8.

The C uses the existing typed inventory layout and canonical lookup/type-query
APIs. There are no register pins, CPU instruction ASM, NOPs, or new casts
between pointers and integers. One empty lookup-merge barrier is retained:
removing it produces 420 rather than 424 bytes and changes 68 compared words.
This raises the main barrier baseline from 899 to 900, within the explicitly
authorized barrier debt. It is not an emitted instruction.

1050368 ASan/UBSan host cases cover every 16-bit item ID, all combinations
of flags and kinds, both active-list identities, and neither/either/both
equipped slots. Additional cases cover signed-byte slot boundaries and
shrinking/growing list bounds during type queries. Hooks change active-list
state during base-item lookup and mutate the record during counting. The
fixture includes the production source unchanged, disabling target-layout
assertions only. Narrow byte-diff and host evidence: `/tmp/pe-slot-equipped/`
(`check.py`, `test.c`, `no-barrier.c`).

`make verify-clean` passes all 340 tests and source, organization and debt
gates. Main retains SHA-1 `452fb033f2eaa4b18aa20a5bca60b8125af3a37b`.
All 191 rebuilt overlays retain their retail SHA-1 values.

### Armor-derived inventory reserve check

`Inv_GetActiveSlotCount` matches all 516 retail bytes at 0x80059A40 with
stock native GCC 2.7.2 and unmodified MASPSX (`-G8`). Its historical name is
retained, but it returns whether free capacity meets the armor-derived reserve,
not a slot count. It resolves the currently tracked armor through the active
list and scans its tail entries. The first entry whose low five bits are
8, 9 or 10 yields a reserve of 1, 2 or 4 respectively; no matching entry or
no resolved armor yields zero. It then computes capped capacity, selects Aya's
inventory and selection-bit storage, counts nonzero signed-halfword item IDs,
and optionally writes the reserve through the output pointer. The list pointer
is reloaded after `Inv_GetAyaSlotLimit`, preserving the retail call boundary.

The two bonus queries are intentional: the second runs only when the first
base-plus-bonus result is below 51. The second result is not clamped again.
Each base-capacity byte is explicitly read after its query, avoiding C's
unspecified operand evaluation order. This source clarification preserves all
retail bytes and was prompted by a host hook that mutates capacity in the query.

There are no new register pins, CPU instruction ASM or NOPs. The existing
inline item lookup requires one empty merge barrier, raising the main barrier
baseline from 900 to 901. The function reuses the two capacity-address views
already required by `Inv_CheckFreeSlotCapacity` and `Inv_CompactActiveListSlots`.
Their four duplicated private alias declarations move to two shared declarations
in `inventory_slots.h`; these are explicitly still matching debt. The debt
tracker now counts shared-header aliases once, in the main scope, including the
previously uncounted AKAO queue alias. Main aliases therefore move from 24 to 23
(24 - 4 + 2 + 1), and file-local extern declarations decrease by four. This is
source deduplication plus corrected accounting, not elimination of the two
address-reload constraints. A regression test verifies that moving an alias
to a header does not erase its debt or multiply it by the number of users.

376320 ASan/UBSan host cases exercise all 16-bit item IDs, signed tracked-slot
bounds, every tail byte value at each position for lengths 0..11, first-match
precedence, capacity/occupancy boundaries, null output pointers, negative item
IDs, and callbacks changing capacity and active-list state. The production
function and aliases are used unchanged; host adaptation disables target-layout
assertions and gives the capacity fixture the explicit assembly symbol name
required by the Mach-O host. Evidence: `/tmp/pe-active-slot-count/`
(`check.py`, `test.c`, source-shape and compiler-option trials).

`make verify-clean` passes all 341 tests and source, organization and debt
gates. Main retains SHA-1 `452fb033f2eaa4b18aa20a5bca60b8125af3a37b`, including
the existing users of the centralized capacity aliases.
All 191 rebuilt overlays retain their retail SHA-1 values.

### Full selected-record snapshots and restoration

`Inv_InitWayneStorage` and `Inv_BuildStorageDisplay` match all 644 and 840
retail bytes respectively, adding 1484 bytes of semantic C. Their inherited
names are retained; the observed operations snapshot and restore the complete
32-byte records for the two remembered selections. The copies are ordinary
`ItemDataRecord` assignments. Stock native GCC 2.7.2 emits the retail unaligned
load/store sequences without instruction ASM, register pins, barriers or NOPs.
Initialization also selects the scratch buffer and clears its two state words;
their further meaning is not inferred from these stores alone.

Both functions join the two existing selection helpers physically between them
in `Inv_SelectionState.c`. All 1748 bytes of the contiguous range
0x80059C44..0x8005A318 match, preserving each function address. Shared selection
state and the snapshot/restore call relationship support this boundary; it is
not a claim that an original object file has been recovered. The inline lookup
uses a normal merged result, with no optimizer barrier. Restoration shares a
private list-selection helper; snapshot initialization retains its two calls
to `Inv_RestoreSelection`, exactly as in retail.

The shared inventory header now owns the selection and snapshot declarations,
removing eleven file-local extern declarations. Item-list pointers use the
existing signed-halfword inventory type, supported by the `lh` ID loads; bitset
pointers use the existing unsigned-word bitset type. These unify previous
pointer-only declarations, without changing widths, addresses or emitted code.
The old three-element remembered-index declaration overlapped the storage-flag
array: the observed slots are at D090/D094 and D098/D09C, and helper bounds
permit only indices 0 and 1. The header therefore uses incomplete array views
for indexed addressing and the four existing scalar symbols for fixed slots.
These are overlapping views of the same state, explicitly documented, not four
additional objects. A two-element sized array causes indexed stores to use the
small-data expansion and shortens the unit by eight bytes; retaining incomplete
views reproduces the retail full-address indexed accesses. No new linker
assignment, ASM symbol alias or type override is introduced.

219218 ASan/UBSan host cases cover every pair of the 392 supported item IDs,
all 65536 raw halfword IDs in the lookup, selection bounds, both remembered
storage flags, absent/present override lists, and all bytes of each record.
Distinct edits to the two snapshots verify restore direction and order even
when both selections resolve to the same record. A callback changes a remembered
index during the slot-limit query, verifying its subsequent reload. Snapshot
and restore callers retain the retail precondition that both selected records
resolve; the null-result cases are tested on the lookup without dereferencing
null. Host adaptation maps the four scalar state views to their corresponding
array elements and uses the host's built-in offsetof; production function bodies
are included unchanged. Evidence: `/tmp/pe-storage-unit/` (`check.py`, `test.c`),
with isolated 644/840-byte proofs in `/tmp/pe-wayne-storage/` and
`/tmp/pe-storage-restore/`.

`make verify-clean` passes all 341 tests and source, organization and debt
gates. Main retains SHA-1 `452fb033f2eaa4b18aa20a5bca60b8125af3a37b`.
All 191 rebuilt overlays retain their retail SHA-1 values.

### Armor change with capacity check and rollback

`Inv_GetSlotItemData` matches all 948 retail bytes at 0x8005968C using stock
native GCC 2.7.2 and unmodified MASPSX (`-G8`). Its inherited name is retained:
the function changes equipped armor and returns a status, rather than returning
an item-data pointer. It compares the old and selected armor's slot reserves,
publishes the selected slot, checks capacity for the reserve difference, and
always runs compaction. Exactly status 1 queues an opcode-3 command containing
the previous armor record and calls `Inv_SetActiveList(3, 0)`. Every other
status restores the saved equipped-slot byte and is returned unchanged.

The old armor may be unresolved, yielding zero reserve, but the newly selected
record retains retail's valid-record precondition. The filtered-index helper
returns slot zero for an out-of-range index, matching the executable. The
previous slot and rollback record are read again after resolving the new armor,
and the filtered slot is read again before publication. These repeated reads
are preserved because lookup calls can change inventory state; the earlier
reserve scan's pointer is not silently reused for rollback.

The function joins the adjacent 516-byte capacity checker in
`Inv_ArmorCapacity.c`. The complete contiguous 0x8005968C..0x80059C44 range is
1464/1464 bytes, with both original entry addresses preserved. Their shared
lookup retains the existing single empty barrier definition, now expanded in
both callers; the source barrier baseline remains 901. The second lookup
variant retains one `$16` pointer pin, expanded twice in the armor-change
function. Main pins increase from 1030 to 1031. Removing this pin in the tested
source shape produces 952 bytes instead of 948; ordinary `register` and a
volatile-pointer trial also fail to match. There are no new CPU instruction ASM,
NOPs, symbol aliases, file-local externs or pointer/integer casts. The existing
typed opcode-3 rollback payload holds an `ItemDataRecord *`.

637990 ASan/UBSan host cases cover every supported old/new item-ID pair plus
missing old armor, signed-byte equipped-slot boundaries, invalid filtered-index
fallbacks, every modifier byte at every position for tail lengths 0..11, and
status values 0, 1, 2, -1, INT_MIN and INT_MAX. Hooks verify the capacity argument,
publication before the check, unconditional compaction, success-only queue and
list calls, the rollback payload, and restoration after callbacks change the
tracked slot. Lookup hooks change the tracked and filtered selections between
lookups to verify that the later reads and rollback pointer are retained.
The merged checker also passes its previous 376320 reserve/capacity cases.
Host fixtures extract the production functions and shared helper unchanged
except for removing the target register annotation; target-only command-layout
assertions are disabled on the 64-bit host. Evidence:
`/tmp/pe-armor-switch/` (`check-unit.py`, `test.c`, no-pin trials) and
`/tmp/pe-active-slot-count/test.c` with its regenerated production excerpt.

`make verify-clean` passes all 341 tests and source, organization and debt
gates. Main retains SHA-1 `452fb033f2eaa4b18aa20a5bca60b8125af3a37b`.
All 191 rebuilt overlays retain their retail SHA-1 values.

### Inventory/storage exchange and explicit row-index forwarding

`Inv_RebuildWithSlotLimit` matches all 1352 retail bytes at 0x80058FEC with
stock native GCC 2.7.2 and unmodified MASPSX (`-G8`). Its inherited name is
retained; the observed routine exchanges inventory/storage entries. It selects
Aya's list first. Equal mode-0x34 endpoints exchange raw storage entries; equal
mode-0x33 endpoints resolve the filtered slots, exchange Aya entries, and update
both tracked equipped-slot bytes independently. Mixed paths check the Aya slot
with `Inv_IsSlotEquipped` and reject storage-record kinds 19..21, then set the
swap-return flag and rebuild the list. Failure returns zero without exchanging
entries or rebuilding. The two conditional record lookups remain separate:
callbacks may change the record before the second kind test.

The three XOR assignments are intentional retail behavior, including clearing
a slot when both addresses are identical. They are not replaced by an ordinary
temporary-value swap. Filtered indices outside the valid range resolve to slot
zero; raw storage indices and record dereferences retain their retail validity
preconditions. A callback that changes the active-list pointer during the initial
slot-limit query is respected by subsequent accesses.

The function joins its two physically adjacent helpers in `Inv_InventorySwap.c`:
`Inv_GetWayneListItemByIndex` (60 bytes) and `Inv_IsSlotEquipped` (424 bytes).
The entire 0x80058E08..0x80059534 range matches all 1836 bytes and preserves
all entry addresses. The swap's ordinary local copies in `SwapAyaSlots` suffice
for the retail register assignment. All experimental register pins and empty
barriers were removed from the new function; even ordinary `register` keywords
were removed. The existing eligibility helper's one empty barrier is unchanged.
Pins, barriers, NOPs, ASM aliases and pointer/integer casts do not increase.
The shared inventory header replaces two file-local extern declarations.

Canonicalizing the filtered-slot getter exposed a real source-level omission
in `menu_draw.c`: it declared a zero-argument pointer-returning getter and
relied on the incoming row index remaining in a0. `Menu_DrawInventoryItemRow`
now takes the index explicitly, passes it to the getter, and uses its integer
slot result for selection, rendering and equipped-slot queries. The armor-row
adapter similarly forwards its index explicitly. `Menu_DrawWeaponList.c` uses
the correct `void (*)(int)` callback signatures and a widget pointer through
the shared item-row header. The retail renderer passes the calculated row index
in a0 at 0x8006361C..0x80063620. These are argument/type repairs, not additional
newly decompiled functions; their byte identity is checked by the full main SHA.

207618 ASan/UBSan swap cases cover all kind bytes, both mode-0x33/0x34 paths,
all pairs of eight slot positions, matching addresses, signed tracked-slot
boundaries, rejected eligibility, filtered-index fallbacks, active-list mutation,
and a record-kind change between the two lookup calls. The resolver is separately
exercised on all 65536 halfword IDs. The merged eligibility helper passes its
previous 1050368 cases. Another 327680 host cases pass every signed-halfword
row index through the actual weapon/armor list adapters and row callbacks,
checking the mapped integer slot and dimming/render/shadow call order. Host
fixtures extract or include production function bodies unchanged and disable
only target-layout assertions. Evidence: `/tmp/pe-inventory-swap/`
(`check-unit.py`, `test.c`, `test-rows.c`, plain-source and constraint-removal
trials) and `/tmp/pe-slot-equipped/` with its regenerated production excerpt.

`make verify-clean` passes all 341 tests and source, organization and debt
gates. Main retains SHA-1 `452fb033f2eaa4b18aa20a5bca60b8125af3a37b`, including
the explicit row-index forwarding repairs.
All 191 rebuilt overlays retain their retail SHA-1 values.

### Inventory modifier/stat transfer and selection unit (0x8005A318)

`Inv_TransferItemBetweenLists` matches all 3300 retail bytes at 0x8005A318
with stock native GCC 2.7.2 and unmodified MASPSX. It transfers one modifier
or all three signed bonus stats between remembered item selections, optionally
consumes a type-12/13 tool, and optionally removes the donor. The historical
entry name is retained; the function returns void and takes four arguments.
Its caller now uses the shared inventory prototype. The caller is not counted
as another newly decompiled function.

The function extends the contiguous `Inv_SelectionState.c` unit containing the
two record snapshots, remembered selection accessors, and snapshot restoration.
The full 0x80059C44..0x8005AFFC range matches all 5048 bytes, preserving every
entry address. The public selection-restoration entry and the transfer's inline
restoration share one implementation. Two lookup result shapes remain: snapshot
copies need the merged output, while type queries use the early return. They
have identical item-ID rules but different stock-compiler register behavior.

The stat comparisons use signed bonuses and cap only the upper result at 999.
Their stored sums explicitly convert the operands to u16. Since the destination
is a halfword, these conversions preserve the low 16 result bits for all inputs,
including negative bonuses and aliasing donor/receiver records. Two ammunition
clamps use the same distinction between signed comparison and halfword storage.
This source typing reproduces both the retail register copies and the full
160-byte frame; there is no invented stack padding. The unsigned modifier-mask
copy, scoped list comparisons, tracked-slot accessors, and declaration order
in the ammunition-room calculation finish the match without register pins or
empty barriers. There is no CPU/GTE ASM, NOP, postpass, or compiler modification.

Modifier replacement first searches for the same nonzero high-bit group, then
for an entry whose low five bits are zero. A full receiver reports notification
7 but still proceeds to the tool/donor-consumption phase. The donor byte is
cleared after the receiver write, including aliased records. Stat transfer
clears all donor bonuses after updating the receiver. Non-armor excess ammo is
limited by receiver capacity; the remainder goes to the category ammo record.
When removing the donor, its remaining ammo is also returned to that record.
Deleting an equipment ID clears record byte 0, not its flags at byte 5.

The active list and tracked slots are re-read at the same callback boundaries
as retail. Removing equipped armor queries reserve before deletion, clears its
tracked index, checks capacity, compacts, and synchronizes equipment. Subsequent
weapon/armor replacement observes callback changes, including a different active
list or a restored armor index. Selected records and slot indices must be valid;
deleted IDs >= 256 must name valid equipment records. The retail weapon-
replacement loop has no null-record guard; tests use
resolvable entries on that path rather than silently adding a guard to the C.

Debt review for this promotion: the byte-pointer-arithmetic baseline increases
from 324 to 328. These four expressions select 32-byte category ammo records in
two source paths. Explicit signed shifts retain retail's subtract-then-shift
address formation; ordinary typed array indexing folds the subtraction into a
different base address. The expressions retain `ItemDataRecord` at every field
access and are recorded as debt, not concealed behind aliases or tool changes.
Pins, barriers, NOPs, pointer/integer casts, field macros, and ASM counts do not
increase. No source-organization baseline changes are needed.

The actual production unit passes 100000 ASan/UBSan transfer cases, covering all
four flag combinations, capped signed stats, halfword overflow, aliasing records,
full modifier slots, tool consumption, donor deletion, and all donor kind bytes
in the no-cost paths. Another 65624 cases cover every raw halfword ID, selection
overrides, both remembered selection indices, callback-driven active-list changes,
and weapon/armor replacement at every index and with no matching item. The older
snapshot/restoration routines retain their 219218 host cases using an unchanged
production excerpt. Host fixtures disable target ABI assertions only; there are
no pins or assembly statements to strip. These tests use an independent C model
and callback fixtures; exactness is established separately by the retail bytes.

Evidence: `/tmp/pe-transfer/` (`target.s`, `check.py`, `check-unit.py`, source-shape
trials, `test-production.c`, `test-selection.c`, and verification logs).
`make verify-clean` passes all 341 tests, source/debt/organization gates, and main
SHA-1 `452fb033f2eaa4b18aa20a5bca60b8125af3a37b`.
All 191 rebuilt overlays retain their retail SHA-1 values.

### Kind-filtered inventory rows and explicit row count (0x80058C4C)

`Inv_TransferItemAlt2` matches all 444 retail bytes with stock native GCC 2.7.2
and unmodified MASPSX. Despite its historical name, it selects Aya's list,
collects indices selected by a kind bitmask, publishes the partial count,
rebuilds selection state, then appends empty slots and returns the final count.
The output cursor survives the callback, while active-list pointer and limit
are re-read for the second pass. A callback's overwrite of the count does not
replace the cursor-derived final count.

The entry is merged with the contiguous filtered-index getter, slot eligibility
check and swap operation in `Inv_InventorySwap.c`. The complete
0x80058C4C..0x80059534 range matches all 2280 bytes and preserves entry addresses.
The new bounded lookup reuses the swap's existing raw-ID resolver; it does not
copy another resolver or add a barrier. The old eligibility helper's barrier
is unchanged. All debt and source-organization baseline counts remain unchanged.

The return type is int, not void. The retail `Menu_OnItemDiscardConfirm` object
calls the function at offset 0x98 and forwards v0 into the item-count argument
of `Draw_SetPrimCallback` at offset 0xA8. Returning `D_8009D044` explicitly retains
the exact function bytes. Both the discard menu and sorting code now use the
shared inventory declaration; the old contradictory void declaration is removed.
These caller declaration repairs are not credited as new decompilations.

The C shift requires resolved item kinds below 32. Tests cover all 32 such kinds
and the sign bit of the signed mask. An explicit `& 31` would broaden this C
contract but adds an instruction with the stock compiler, so it is not silently
substituted for the retail source shape. Invalid raw item IDs resolve to kind
zero. With mask bit zero set, an empty slot can be collected in both phases;
that behavior is retained. The known callers use 0xF400 or 0x3803FE, both with
bit zero clear. Tests keep all generated output within the 50-slot buffer.

214531 ASan/UBSan cases include the production unit unchanged, exercise all
65536 raw halfword IDs, all legal kinds, individual and combined mask bits,
negative/zero initial limits, callback redirection of the initial/second list,
partial-count observation, count corruption, output-prefix mutation, final
return values, and untouched output tails. Only target-layout assertions are
disabled in the host fixture. The existing matching functions are additionally
covered by the whole-unit byte comparison. Evidence: `/tmp/pe-filter-kinds/`
(`target.s`, `check.py`, `check-unit.py`, `test-production.c`, and build logs).

`make verify-clean` passes all 341 tests and source/debt/organization gates.
Main retains SHA-1 `452fb033f2eaa4b18aa20a5bca60b8125af3a37b`, including both
caller declaration repairs.
All 191 rebuilt overlays retain their retail SHA-1 values.

### Ammunition-compatible selection masks (0x800562A4)

`Inv_BuildCompatibleWeaponBitset` matches all 2176 retail bytes with stock native
GCC 2.7.2 and unmodified MASPSX. It clears the active selection mask, selects
compatible records except the source slot, counts the selected bits, and repeats
on the other logical list when available. Only the original list excludes the
source index. It restores the original logical list, not an arbitrary saved raw
pointer. The source slot must resolve to a record, as in the original unchecked
kind load. All 256 kind bytes are supported by the reconstructed arithmetic.

Kinds 1..7 map to category max(kind - 4, 1); kinds >=19 map to kind - 18; other
kinds map to zero. Sources of kind 19..21 select any record in the same category;
other sources select only kind 19..21 records in that category. The category is
cached before list-switch callbacks. Aya slot-limit callbacks can redirect the
active pointer or remove the override; subsequent iteration/restoration observes
these changes.

The C uses no pins, barriers, NOPs, instruction ASM or extra compiler flags.
Sharing the initial and loop item pointer removes the trial's last register pin.
The five bounded lookups share one raw-ID resolver. The two halfword candidate
kind temporaries retain the retail byte-narrowing instructions; replacing them
with byte or word temporaries removes those instructions. Conditional category
expressions preserve the split zero-category paths; an inline category function
changes those paths. The bit counts compare unsigned masked words with zero.

The key-item resolver retains one signed offset into a byte table before
accessing the shared `ItemDataRecord` layout. This is byte-pointer arithmetic
debt. The current regex also counts four boolean-to-u32 casts as
`pointer_integer_casts` (737 -> 741 in main), although all four operands are
boolean expressions, not pointers. These counted conversions make shifts at
bit 31 unsigned and are retained explicitly in the baseline. Caller declarations
move to the canonical `int (int)` prototype in `inventory.h`.

218192 ASan/UBSan cases include the production file unchanged (only target ABI
assertions are disabled). They cover every pair of kind bytes on both lists,
every raw halfword destination ID, every valid source ID, random 50/100-slot
lists, high mask bits, alternate active pointers, absent/present overrides,
callback redirection/removal, nonpositive callback limits, restored globals,
returned counts and complete mask buffers. The independent host model verifies
semantics; a separate linked-object comparison establishes the retail byte match.
Evidence: `/tmp/pe-compatible/` (`target.s`, `check.py`, source-shape trials,
`test.c`, and acceptance logs).

`make verify-clean` passes all 341 tests and source/debt/organization gates.
Main retains SHA-1 `452fb033f2eaa4b18aa20a5bca60b8125af3a37b`, including the
shared caller prototypes.
All 191 rebuilt overlays retain their retail SHA-1 values.

### Comparison snapshots in the ammunition unit (0x80056C40)

`Inv_BuildDisplayFromList` matches all 888 retail bytes with stock native GCC
2.7.2 and unmodified MASPSX. It selects the two requested logical lists, retains
the resolved record pointers at D_8009D070/D_8009D074, copies the two complete
32-byte records into the comparison snapshots, then clears tailData[10] and
reserveAmmo in both copies. Both retained pointers must be valid at copy time.
The copies remain sequential: the first destination write can affect the second source when they
alias. The second list-switch callback can replace the retained first pointer,
and the snapshots observe that replacement. The final active list is the second
selection. Menu_AmmoSpendPanel already called the function as void with four
integer arguments; it now uses the shared declaration.

The snapshot function shares `Inv_AmmoSelection.c` with the contiguous
compatibility-mask builder, Spend_Ammo, category-ammo accessor and the selected
alternative search described below. The complete unit matches all 4012 bytes;
the snapshot contributes 888 new bytes. The bounded lookup wrapper and compatibility loops share one raw-ID resolver; logical list
restoration is shared too. The record pointers and existing halfword category
table now have header ownership. There are no new pins, barriers, NOPs, ASM,
compiler flags, field macros or casts.

The production merged unit passes 51578 ASan/UBSan snapshot cases, including all
392 mapped source IDs, 50/100-slot boundaries, every combination of logical list
selectors, alternate list pointers, callbacks removing the override or replacing
the first record pointer (including rescue of a null first lookup), and
sequential source/destination aliasing. The prior compatibility builder retains all 218192 host cases using the merged production
file unchanged. Only target ABI assertions are disabled in these host fixtures.
Exactness is independently established by the linked 4012-byte retail comparison.
Evidence: `/tmp/pe-compare-snapshot/` (`target.s`, `check-expanded.py`,
`test-unit-snapshot.c`, `test-unit-compatible.c`, and acceptance logs).

### Selected alternative search (0x8005600C)

`Inv_InitSlotDisplay` matches all 664 retail bytes with stock native GCC 2.7.2
and unmodified MASPSX. It searches the requested logical list's selection bits
for a slot other than the caller's current index. If none is found, it searches
the other logical list without excluding that index. Success updates the index
and, on the second pass, negates the logical list selector; failure writes -1
to the selector while retaining the index. It finally restores the original
logical list. The pointers to the selector and index may alias; stores retain
the retail order. Slot-limit callbacks may change these pointed-to values,
redirect Aya's list pointer or remove the storage override, all observed by the
following operations. The result is conveyed through the pointers, not a return
value; Menu_EquipScreen now uses the canonical void(int *, int *) declaration.

This entry extends the same `Inv_AmmoSelection.c` unit to
0x8005600C..0x80056FB8: all 4012 bytes and five function entry addresses match.
The two newly reconstructed functions account for 1552 bytes. The search reuses
the existing list restoration helper and uses ordinary for loops and unsigned
bit tests. No pins, barriers, NOPs, ASM, gotos, field macros or casts are added.
The header move lowers main externs_in_c from 3668 to 3667; other baseline
counts are unchanged.

73056 ASan/UBSan cases include the production merged unit unchanged. They cover
all pairs of four-bit masks, non-boolean selectors, excluded indices, aliased
output pointers, random 50/100-slot masks, all bit positions including bits
outside the active limit, nonpositive limits, three initial pointer identities,
absent/present overrides and all callback combinations described above. They
check final outputs, restored globals and untouched bitsets against a separate
model. The expanded production file also passes the 51578 snapshot cases and
218192 compatibility cases. Evidence: `/tmp/pe-find-selection/` (`target.s`,
`check.py`, `test-production.c`), and `/tmp/pe-compare-snapshot/check-expanded.py`
for the complete 4012-byte comparison.

`make verify-clean` passes all 341 tests and source/debt/organization gates.
Main retains SHA-1 `452fb033f2eaa4b18aa20a5bca60b8125af3a37b`, including both
caller declaration changes and the five-function translation unit.
All 191 rebuilt overlays retain their retail SHA-1 values.

### Selectable flag mask and unsigned bit accessors (0x80055E14)

`Inv_SortSlotsByPriority` matches all 416 retail bytes with stock native GCC
2.7.2 and unmodified MASPSX. Its historical name is misleading: it clears the
active selection bitset, excludes the tracked weapon and armor indices, calls
Inv_IsSlotSelectable on the other slots, and selects resolved records whose
flag bits 0xE0 are all clear. The tracked-index exclusion also applies when the
active list is storage. Callback changes to the active pointer, limit, bitset
pointer and tracked armor index affect subsequent operations. A null lookup
retains the old bitset word; the nullable conditional expression preserves the
retail branch/store structure that a logical-AND expression collapses.

The entry and both contiguous 44-byte bit accessors join `Inv_AmmoSelection.c`.
The full 0x80055E14..0x80056FB8 range matches all 4516 bytes and eight entry
addresses. Only the 416-byte function is newly decompiled. The former signed
int pointer in the accessor files becomes the shared u32 pointer D_8009D058;
its tentative definition remains in the merged unit. Both accessors use
unsigned masks, including bit 31, and retain their exact retail bytes. Their
old signed left shifts are no longer necessary. The caller uses the canonical
void(void) declaration from inventory.h.

There are no new pins, barriers, NOPs, ASM, gotos or compiler flags. One new
boolean-to-u32 conversion is counted by the existing pointer_integer_casts
regex (741 -> 742 in main); its operand is a boolean, not a pointer. The old
file-local bitset extern is removed (3667 -> 3666); the lowered count is retained
in the baseline. These are the only baseline count changes.

172336 ASan/UBSan cases include the production file unchanged and cover all raw
halfword IDs, all flag bytes at every index with each tracked-index exclusion,
callback order, redirected pointers/limits/bitsets, callback changes to tracked
indices, and nonpositive limits/clear-word counts. Another 2048 cases verify
both unsigned accessors at every bit 0..127 with zero, full and random words,
including preservation of unrelated bits. The prior 51578 snapshot, 218192
compatibility and 73056 selection-search cases also include the expanded unit.
Only target ABI assertions are disabled. Exactness is independently established
by the linked 4516-byte retail comparison. Evidence: `/tmp/pe-select-flags/`
(`target.s`, `check-unit.py`, `test-unit.c`, `test-snapshot.c`,
`test-compatible.c`, `test-search.c`, and acceptance logs).

`make verify-clean` passes all 341 tests and source/debt/organization gates.
Main retains SHA-1 `452fb033f2eaa4b18aa20a5bca60b8125af3a37b`. The full linked
symbol table resolves both D_8009D058 and g_InvSelectionBits to absolute address
0x8009D058; the four pointer bytes at file offset 0x8D858 remain zero, identical
to retail. This verifies the common-symbol/type migration in the actual link.
All 191 rebuilt overlays retain their retail SHA-1 values.

### Menu-dependent selection masks (0x80055760)

`Inv_RebuildSelectableMask` matches all 1716 retail bytes with stock native
GCC 2.7.2 and unmodified MASPSX. It first scans the available logical lists for
weapon kinds 1..7 and armor kind 9. A second pass clears each list's bitset and
selects records according to the menu mode, flag bit 0 or 1, consumable effect
and current/max HP. List availability is checked each iteration. The function
restores the original logical list after both passes. Slot-limit/menu callbacks
can change the override, list pointer, limit, bitset pointer, battle flag and HP;
subsequent operations observe those changes.

The nonzero menu mode excludes consumable effect 2. In zero mode, consumable
effects 4..6 require a weapon and effects 12..14 require armor. Regardless of
kind, item IDs 6..10 are excluded when current HP is at least maximum HP.
Capturing flags in an int before selecting the flag bit preserves the retail
load scheduling and arithmetic shift. ConsumableEffect reads the first byte of
the existing bonusStats member through a character pointer, matching the byte
view already used in Inv_ItemActions without a numeric field offset or aliasing
violation. The existing AyaSaveState layout supplies current_hp/max_hp through
a typed D_800C0E00 declaration; no new layout or field macro is invented.

The weapon-kind variable shift requires resolved kinds below 32. The USA retail
base table supports that contract: Scene_LoadSceneData loads PE.IMG sectors
72..87 to 0x800A8028; Item_TableLookup's offsets at +12/+16 are 0x35E0..0x55C0,
covering 255 records whose kind bytes are all 0..21. The table SHA-1 is
0ef3b123b2571b6ddadfdcb5dc4defb4553c3888. This is evidence for that table, not a
claim that all runtime equipment mutations or arbitrary save records have been
exhaustively audited. Host fixtures explicitly use the valid 0..31 kind domain.
Invalid raw IDs resolve to kind zero.

The function joins the shared helpers in `Inv_AmmoSelection.c`; the full
0x80055760..0x80056FB8 range matches all 6232 bytes and nine entry addresses.
Only 1716 bytes and one function are newly decompiled. No pins, barriers, NOPs,
ASM, gotos, extra flags, file-local externs or tracked debt counts are added.
Canonical declarations live in aya.h and menu_state.h.

226608 ASan/UBSan cases include the production merged unit unchanged and compare
both passes against an independent model. They cover every raw halfword ID,
all flag/effect byte combinations in both menu modes, equipment availability,
HP conditions, random 50/100-slot lists, nonpositive limits, and combinations
of callback mutations listed above. They check complete bitsets, final globals,
HP/flag state and callback counts. The existing 517210 host cases for the other
functions also pass with the expanded production file. Only target ABI asserts
are disabled. Exactness is separately established by the 6232-byte comparison.
Evidence: `/tmp/pe-rebuild-mask/` (`check-unit.py`, `test-production.c`, older
function fixtures, `inspect-retail-table.py`, `table-evidence.txt`, build logs).

`make verify-clean` passes all 341 tests and source/debt/organization gates.
Main retains SHA-1 `452fb033f2eaa4b18aa20a5bca60b8125af3a37b`. The full linked
symbol table resolves typed D_800C0E00 to 0x800C0E00, the same address as the
existing g_AyaSaveTotalExp alias, without a new manual linker assignment.
All 191 rebuilt overlays retain their retail SHA-1 values.

### Random ammunition-pool withdrawal (0x800553A4)

`Inv_RollRandomItemType` matches all 620 retail bytes with stock native GCC
2.7.2 and unmodified MASPSX. Null argument pointers leave state untouched.
A zero type counts nonempty ammunition fields in the first three records at
D_800A1E64 and, if that count is nonzero, chooses 1 + count * randomByte / 256.
Retail writes a local list of available indices but never reads it; this is
preserved, including the fact that the chosen type is not remapped through it.
Types outside 1..3 do nothing further. Zero amount draws a random fraction of
the chosen stock; negative amount requests that percentage of the stock.
The result is narrowed to signed 16 bits, clamped against stock using a signed
amount comparison, and subtracted from the unsigned 16-bit stock. Narrowing
and wraparound quirks are preserved, rather than imposing new domain checks.

The stock accesses use the existing ItemDataRecord.ammo field. This structure
also recovers retail's indexed first loop without pins or barriers. The random
generator is a single 521-byte array: on wrap it XORs each byte with the byte
489 positions ahead modulo 521, in order, so later iterations see earlier
writes. The three retail address labels are views within that one array, not
independent buffers. The initializer in Inv_SetDefaultItemId confirms the
521-byte extent and initializes the cursor to 520. This reconstruction requires
a valid cursor in 0..520 on entry. No new layout, ASM, pins, barriers, NOPs,
gotos, compiler flags or tracked debt is introduced. Canonical declarations
live in inventory.h and random.h; the battle caller uses the typed prototype.

284448 ASan/UBSan cases include the production function unchanged and compare
an independent model's arguments, complete record array, random state and
cursor. They cover all 65536 type bit patterns, all 65536 amount bit patterns,
every valid cursor, stock boundaries, randomized records and null/aliased
argument pointers (including signed/unsigned views of stock). Only target ABI
assertions are disabled for the host harness. Linked retail comparison proves
620/620 bytes independently of those tests. Evidence: /tmp/pe-roll-random/
(target.s, check.py, test.c, and acceptance logs).

`make verify-clean` passes all 341 tests and source/debt/organization gates.
Main retains SHA-1 `452fb033f2eaa4b18aa20a5bca60b8125af3a37b`, including the
battle caller after replacing its untyped argument declaration.
All 191 rebuilt overlays retain their retail SHA-1 values.

### Random selection and item messages (0x8005485C)

`Inv_PickRandomItem` (556 bytes) and `Inv_GetItemEffectData` (624 bytes) form
one 1180-byte `Inv_RandomSelectionText.c` unit, exact with stock native GCC
2.7.2 and unmodified MASPSX. They add no pins, barriers, NOPs, ASM, gotos or
compiler flags. Lookup helpers address equipment and ammunition as existing
typed record arrays, including raw IDs 512..520, without byte-pointer arithmetic.
The historical function names are retained for callers.

The first function packs indices of valid records with flag 0x80, excluding
tracked weapon/armor indices only for the Aya list. It observes list, limit,
record and tracking changes made by base-data lookup callbacks. It clears
D_8009D068, publishes the count in D_8009D040, and returns zero without advancing
the RNG when no eligible record exists. Otherwise it draws one byte from the
521-byte generator and returns the raw item ID at the chosen packed index.
The original ignores its integer argument; the prototype retains it because
the battle caller computes and passes r / 100. The generator requires a valid
cursor in 0..520, and the caller must provide valid lists and enough packed
output capacity. The incomplete-array declarations do not assert a new extent.

The second function returns an FF-terminated message pointer, not integer
item-effect data. Item zero selects table4[19]; mode >= 2 selects table4[18].
Otherwise it concatenates table8[item - 1] and table4[mode + 16], reversing
order when D_8009D218 is nonzero. It rescans the destination after the second
lookup, preserving callback-visible changes before appending. The caller must
supply valid table indices and terminated strings that fit the shared buffer;
the reconstruction does not add bounds checks absent from retail.

The USA resource loaded by Scene_LoadSceneData from PE.IMG sectors 72..87
contains the string tables. Str_LookupTables uses the resource's word at +4
as its relative base, then offsets +4/+8 to tables at resource offsets 0x44
and 0x800. Their counts are 120 and 255. Table8 payloads are at most 14 bytes;
table4[16..19] payload lengths are 11, 6, 23, 22. Thus normal modes 0/1 yield
at most 26 bytes including FF, and the two fallback messages use 24/23 bytes.
Resource SHA-1: 9c645d27d8bb9c820aa778d4eb7145bd773b8f73. This establishes the
retail table case, not arbitrary runtime replacements or invalid arguments.

Canonical prototypes are in inventory.h/text.h. Battle_ApplyEnemyAttack now
stores the typed return value through a real u8-pointer declaration in battle.h,
removing its synthetic 16-byte alias and integer view. Its compiled instructions
and relocations remain identical. The main file-local extern count falls from
3666 to 3665; the remaining ratchet counts do not change.

The production unit passes 96057 ASan/UBSan selection cases and 31230 text cases.
Selection tests cover every raw halfword ID, random lists, tracked indices,
nonpositive limits, every valid RNG cursor, and callback mutations of the list,
limit, record flags, tracked index, RNG cursor, and null lookup results. Text
tests cover empty/full 31-byte source pairs, all nonterminator byte values,
mode/order branches and mutation by the second lookup. Fixtures provide valid
output capacities and disable only target ABI assertions. Exactness is proved
separately by the complete 1180-byte retail comparison. Evidence:
/tmp/pe-pick-text/ (check.py, test-pick.c, test-text.c, inspect-text-tables.py,
text-table-evidence.txt, caller-before/after.dump, and acceptance logs).

`make verify-clean` passes all 341 tests and source/debt/organization gates.
Main retains SHA-1 `452fb033f2eaa4b18aa20a5bca60b8125af3a37b`, including both
new functions and the typed pointer stores in Battle_ApplyEnemyAttack.
All 191 rebuilt overlays retain their retail SHA-1 values.

### Checking availability of two mod-bearing items (0x80054520)

`Inv_SetupSlotDisplay` matches all 828 retail bytes with stock native GCC
2.7.2 and unmodified MASPSX. Its historical name is retained: it counts records
whose kind bit is enabled by the supplied mask and whose tailCount is nonzero,
then returns whether at least two qualify. It always starts by selecting the
Aya list. It also selects/scans the override list when one remains available
after the first scan and mask bit 0x100 is clear. All loop conditions observe
the current slot limit. Finally it selects the original logical list (determined
by initial pointer equality with Aya's list), observing override availability
and slot-limit callback effects at that point. It does not restore an arbitrary
saved raw pointer, stop after the second match, or clear/write selection bits.

The new function shares typed lookup and selection helpers with the adjacent
selection/message code in Inv_RandomSelectionText.c. The full 2008-byte range
0x80054520..0x80054CF8 and all three entries are exact; only 828 bytes and one
function are new. The local item pointer is assigned in both sides of the
bounds check, preserving retail's branches without pins or barriers. There
are no new ASM, NOPs, gotos, flags, local externs or ratchet changes.

As in other kind-mask queries, resolved kinds must be below 32 for the C shift.
The previously audited USA base table has kinds 0..21. This does not establish
the range of arbitrary runtime mutations or malformed saved items. Tests use
kinds 0..31 and valid list capacities. Signed-mask shifts retain the native
compiler's arithmetic-shift behavior; bit tests are checked independently with
unsigned masks in the host model.

96057 ASan/UBSan cases include the production unit unchanged: every raw halfword
ID, random Aya/override lists and masks, nonpositive limits, and callback changes
to list pointers, limits and override availability, including null base lookup
results. The model checks the return value, final list/bitset pointers and
limits, record state, and callback counts. The prior 96057 selection and 31230
text cases also pass with the expanded production unit (223344 cases total).
Only target ABI assertions are disabled. Exactness is independently established
by the complete 2008-byte retail comparison. Evidence: /tmp/pe-setup-slot/
(check-unit.py, test.c, test-pick.c, test-text.c, and acceptance logs).

`make verify-clean` passes all 341 tests and source/debt/organization gates.
Main retains SHA-1 `452fb033f2eaa4b18aa20a5bca60b8125af3a37b`.
All 191 rebuilt overlays retain their retail SHA-1 values.

### Allocating a copied equipment record (0x80053968)

`Inv_FindSlotByIndex` matches all 480 retail bytes with stock native GCC
2.7.2 and unmodified MASPSX. Despite its historical name, it finds the first
equipment record whose leading byte is zero and the first empty active slot.
If either is unavailable it returns null. Otherwise it copies the complete
32-byte base record, selects Aya's list, and inserts the new record ID using
the slot index found before selection. The slot-limit callback can change the
active pointer; the final write follows that pointer, as retail does.

Typed record assignment and the shared InventoryRuntime layout reproduce the
copy and address reuse. A local index in the record search preserves the
retail intermediate calculation without pins, barriers, ASM or new flags.
The canonical return type is ItemDataRecord *, and its existing caller only
tests it against null.

212160 ASan/UBSan model cases cover every free equipment position (and full),
every free slot in a 50-slot list (and full), limits 0..50, Aya/storage lists,
callback pointer changes, and base sources aliasing equipment. They compare
whole record/list state, result pointers and callback counts/arguments. The
contract requires a valid non-null base lookup for IDs 1..255, valid list
ranges, and a preselected slot valid in the final list. Arbitrary malformed
IDs, negative limits and partial record overlaps are not covered. Only target
ABI assertions are disabled for host testing. Evidence is recorded under
/tmp/pe-allocate-equipment/.

`make verify-clean` passes all 341 tests and source/debt/organization gates.
The complete main executable retains retail SHA-1
`452fb033f2eaa4b18aa20a5bca60b8125af3a37b`, including the caller compiled
against the corrected return type.
All 191 rebuilt overlays retain their retail SHA-1 values.

### Inserting an ammunition pool ID (0x80053B48)

`Inv_WriteSlotById` matches all 484 retail bytes with stock native GCC 2.7.2
and unmodified MASPSX. Its historical name is retained. Kinds 1..5 map to
ammunition category 0, kinds 6..7 to 1..2, and kinds 16..18 to 0..2. Other
kinds return failure without modifying records or slots. For a valid category
it searches the active list for the pool ID, then inserts it into the first
empty slot if necessary. A full list returns failure, but still adds the
source ammunition to the pool. Addition narrows to u16 before clamping to
min(baseStats[2] + bonusStats[2], 999); negative limits retain retail's signed
comparison and final halfword conversion. The source may alias the pool.

The ClampAmmo expression follows the already matched Inv_SelectionState
helper. A single search-key pin to $a3 is retained and counted in debt:
removing it exchanges $a2/$a3 and changes 11 instruction words. There are no
empty barriers, CPU instruction ASM, NOPs or compiler changes.

The function shares FindFreeActiveSlot with the immediately preceding record
allocator in Inv_InsertItem.c. The contiguous 964-byte unit at
0x80053968..0x80053D2C matches exactly; 484 bytes and one function are new.
This groups two inventory insertion operations with shared active-list state
and a common search, without claiming an independently recovered original
object boundary. Both signatures now live in inventory.h. Both ammunition callers use the
canonical declaration; the second caller also drops its stale void-pointer
base-lookup declaration. Its object text and relocations are unchanged.

112200 ASan/UBSan model cases cover all 256 kinds, list lengths 0..50, every
slot position, existing/absent pool IDs, full lists, random signed bonus and
unsigned ammunition values, and source records aliasing pool records. The
model compares complete list/pool state and return values. The preceding
allocator's 212160 cases also pass against the merged unit (324360 total).
Host testing removes the one register binding and target ABI assertions;
arithmetic and control flow are unchanged. Valid record pointers and list
ranges are required; malformed pointer ranges are outside this model.
Evidence: /tmp/pe-add-ammo/ (check-unit.py, test.c, test-allocate.c and logs).

`make -j8 verify-clean` passes all 341 tests and source/debt/organization
gates. Main retains SHA-1 `452fb033f2eaa4b18aa20a5bca60b8125af3a37b`.
The subsequent caller-only prototype correction passes `make -j8 verify`
again (341 tests and unchanged main SHA-1).
All 191 rebuilt overlays retain their retail SHA-1 values.

### Transferring pending item IDs (0x80058454)

`Inv_TransferItemAlt` matches all 540 retail bytes with stock native GCC 2.7.2
and unmodified MASPSX, with no pins, barriers, instruction ASM or new flags.
It selects Aya's list, then walks the pending halfword IDs at D_800A1FD4 using
the current count at D_8009D078. It resolves each ID through the typed base,
equipment or ammunition records. The second kind comparison performs another
lookup: callback changes must not be hidden by caching the first pointer or
kind. Kinds passing both comparisons use Inv_CanAddActiveListItemToAya; a
zero result clears the pending entry. Otherwise IDs below 512 are inserted
into the first empty active slot and cleared from the pending list. IDs of
512 or above outside that kind path remain pending. The active pointer/limit
and pending count are observed again after callbacks, and the function always
finishes with Inv_RebuildSelectableMask.

Canonical declarations live in inventory.h and inventory_slots.h; the menu
caller drops its local extern declaration. Typed record lookup and the free
slot search already established by adjacent inventory work suffice for the
match. This retains the existing split boundary rather than asserting a
recovered original object boundary.

93440 ASan/UBSan model cases include the production source unchanged (only
target ABI assertions disabled). They cover all 256 kinds, pending counts
0..10 and -1, list capacities 0..50, equipment/base/ammunition IDs, and callback
changes to kinds, pointers, limits, counts and pending entries. They compare
complete list/record state, callback counts and an ordered call trace, plus
final selection state after the rebuild callback. Pending IDs must resolve
to non-null records whenever visited; invalid IDs, null base results and
malformed pointer ranges are outside the contract because retail dereferences
the lookup result directly. Callback-mutated counts must stay within the
pending allocation. Fixture size 10 does not establish a global array bound.
Evidence: /tmp/pe-transfer-active-list/ (check.py, test.c and acceptance logs).

`make -j8 verify-clean` passes all 341 tests and source/debt/organization
gates. Main retains SHA-1 `452fb033f2eaa4b18aa20a5bca60b8125af3a37b`.
The main local-extern debt baseline decreases from 3665 to 3664.
All 191 rebuilt overlays retain their retail SHA-1 values.

### Materializing mutable equipment records (0x80058670)

`Inv_ClearDisplaySlots` matches all 636 retail bytes with stock native GCC
2.7.2 and unmodified MASPSX. Despite its historical name, it clears the list
override, selects Aya's list, and converts base equipment into private mutable
records. For each current slot it resolves the item kind (null/invalid lookup
is kind zero). Kinds 1..9 whose subsequently re-read raw slot ID is outside
256..383 allocate the first equipment record with a zero leading byte. If a
record is available, the function copies the full 32-byte base record and
replaces the current slot ID with 256 + record index. A full record table
leaves the slot untouched. It copies the occupancy byte rather than forcing
it nonzero. Lookup and slot-limit callbacks may change pointers, limits,
records or the override; subsequent operations observe those changes.

One empty tied-register barrier on the equipment search pointer preserves
retail's independent base load alongside its separately cached bound. It
emits no instructions and is counted in debt. There are no pins, NOPs or
compiler changes. A separate overlapping D_800C0EAC declaration also matched
in an experiment, but was rejected in favor of keeping pointer comparisons
and subtraction within the single typed InventoryRuntime equipment array.

The adjacent pending-transfer command shares LookupItem in
Inv_InventoryImport.c. Both are called by Menu_SaveLoadNavigate; their
contiguous range 0x80058454..0x800588EC matches all 1176 bytes. Only the latter
636 bytes and one function are new. This grouping follows adjacency, common
state and lookup logic, without claiming recovered original object metadata.
The new public declaration lives in inventory.h; its caller drops a local
extern. Debt changes are +1 empty barrier and -1 local extern.

171312 ASan/UBSan model cases cover every raw halfword ID, all 256 kinds,
every free record position and a full table, active limits 0..50 and -1,
null base results on the kind-query path, and callback changes to pointers,
limits, occupancy and override state. Base sources can alias equipment
records, including the destination. Tests compare complete state and exact
ordered callback traces. The prior 93440 pending-transfer cases also pass
against the combined unit (264752 cases total). Host fixtures remove only
the empty barrier and target ABI assertions, and bind D_800C0E48 to the slots
field of D_800C0E20, reproducing the shared retail address.

The copy path requires a current base ID 1..255 and a non-null full-record
lookup result; arbitrary callback mutations or low kinds on ammunition IDs
can violate that precondition. Slot ranges must remain within valid storage.
Partial record overlaps are outside the tested contract. Evidence:
/tmp/pe-materialize-equipment/ (check-unit.py, test.c, test-transfer.c, logs).

`make -j8 verify-clean` passes all 341 tests and source/debt/organization
gates. Main retains SHA-1 `452fb033f2eaa4b18aa20a5bca60b8125af3a37b`.
All 191 rebuilt overlays retain their retail SHA-1 values.

### Selecting a storage list and its limit (0x800588EC)

`Inv_StepScrollDisplay2` matches all 444 retail bytes with stock native GCC
2.7.2 and unmodified MASPSX, without new pins, barriers, instruction ASM or
flags. Its historical name is retained. A zero argument selects D_800C1EB8
and a limit of 100. A nonzero argument selects D_800C1F80 and scans 82 entries
for ID 516. If absent, it searches the current active list for a resolved
kind-6 item, with null/bounds checks. A found item causes insertion into the
current D_8009D07C list, which callbacks can have changed. The resulting limit
is 80 or 81. Finally the selected pointer/limit become the override, and the
limit is returned. The ASM caller Menu_OpenEquipScreen at 0x8004E810 saves
the returned v0 in s4; returning the limit resolves four final register
allocation differences present in the initial void candidate.

The two scan bounds deliberately differ: the presence scan visits 0..81,
whereas the empty-slot scan visits only 0..80. If those 81 entries are all
occupied, retail overwrites entry 81 without testing it for zero. A preexisting
516 at entry 81 counts as present and prevents insertion. The reconstruction
preserves these operations and does not infer a corrected container policy.

The function extends adjacent Inv_InventoryImport.c and shares LookupItem.
All three entries in 0x80058454..0x80058AA8 match the complete 1620-byte range;
444 bytes and one function are new. This is a grouping by adjacency and common
inventory state, not a claim of recovered object metadata. Canonical data and
function declarations live in inventory_slots.h and inventory.h. Existing
crutch counts, including the materialization helper's one barrier, are unchanged.

215184 ASan/UBSan cases cover every raw halfword ID, all kinds, both argument
branches (including a negative nonzero value), limits 0..50 and -1, every
presence/empty position 0..81 and absence/full cases, null base results and
callback changes to active/selected pointers, limits, kind and override state.
They compare returned limits, complete list/record snapshots and ordered
lookup traces. The previous 171312 materialization and 93440 transfer cases
also pass against the expanded unit (479936 cases total). Host fixtures bind
retail's slot-array aliases, disable target ABI assertions and omit only the
existing empty barrier. Active storage ranges must be valid and non-null
lookup results must point to complete records. Tests keep callback-selected
storage at least 82 entries long; they do not establish a global maximum
for arbitrary callback-mutated lengths. Evidence:
/tmp/pe-select-storage-list/ (check-unit.py and the three test*.c fixtures).

`make -j8 verify-clean` passes all 341 tests and source/debt/organization
gates. Main retains SHA-1 `452fb033f2eaa4b18aa20a5bca60b8125af3a37b`.
All 191 rebuilt overlays retain their retail SHA-1 values.

### Reloading the tracked weapon (0x800574A8)

`Inv_DrawSlotItemIcon` matches all 428 retail bytes with stock native GCC
2.7.2 and unmodified MASPSX. Its historical name is retained: it resolves
tracked slot 0, chooses the corresponding ammunition pool, transfers ammo
and returns the signed amount. Kinds 1..5 select pool 0, 6..7 select 1..2,
kinds 19 and above select kind-19, and other kinds use the fallback record.
The amount is min(pool ammo, min(base capacity + signed bonus, 999) - loaded
ammo). This may be negative: the reconstruction preserves reverse transfers
and halfword wrapping. Pool subtraction happens before the weapon's ammo is
read for addition. When both pointers alias, the two stores cancel modulo
65536 even though the returned amount can be nonzero.

Explicit signed category-index conversions preserve retail's subtraction
before scaling instead of folding the offset into the pool address. Separate
locals for the second capacity calculation reproduce the allocation without
pins or barriers. No new flags, instruction ASM, NOPs or crutch debt are added.
The canonical prototype in inventory.h replaces the battle caller's local
declaration; that caller returns the transferred amount.

The reload operation joins the contiguous item-action unit, retaining both
lookup contracts: raw-ID lookup for reloading and bounds-checked tracked-slot
lookup for the existing actions. All five entries in
0x800574A8..0x80057B70 match 1736 bytes, and the dispatcher jump table at
0x800112A4 matches all 56 bytes. Only 428 code bytes and one function are new.
This grouping follows adjacency, common inventory state and item operations,
without claiming recovered original object metadata.

306368 ASan/UBSan cases cover every halfword ammo value, all supported raw
record IDs and kinds 0..27, signed capacity extremes, source/pool/fallback
aliasing, and base-lookup callbacks changing kind, ammo, tracked selection,
active pointer and limit. They compare complete record/list state, return
values and callback arguments/counts. The previous 231424 dispatcher cases
also pass against the expanded production unit (537792 total). Host tests
disable target ABI assertions only and discard uncalled functions at link
time; the tested production code and existing empty barrier are unchanged.

The tracked index must initially lie in the active range and resolve to a
non-null record, as retail dereferences it directly. Kinds beyond 27 index
outside the established nine-record pool array and are not in the tested
contract. Full-record aliases are tested; arbitrary partial overlaps are not.
Evidence: /tmp/pe-reload-weapon/ (check-unit.py, test.c, test-dispatch.c, logs).

`make -j8 verify-clean` passes all 341 tests and source/debt/organization
gates. Main retains SHA-1 `452fb033f2eaa4b18aa20a5bca60b8125af3a37b`.
All 191 rebuilt overlays retain their retail SHA-1 values.

### Committing the ammunition comparison (0x80057094)

`Inv_StepScrollDisplay` matches all 1044 retail bytes with stock native GCC
2.7.2 and unmodified MASPSX. The historical name is retained. If either
comparison snapshot's ammo differs from its current item, the function
allocates opcode 4, records the item pointers, returns the left snapshot's
reserve ammo (or the right's when the left reserve is zero) to the pool
selected by tailData[10], and records the previous ammo for rollback.
It then copies both complete snapshots back in left-to-right order and
notifies Inv_SetActiveList. The mode is 5 when the tracked weapon is either
updated item, otherwise 7; mode 7 does not consume the selection buffer.

Pool ammo is clamped before halfword narrowing: min(current + reserve,
min(base capacity + signed bonus, 999)). Negative capacity and wrapping are
preserved. The local result is u16, with signed comparison and unsigned
narrowing in the selected capacity expression. This source shape reproduces
the original allocation without pins, empty barriers or instruction ASM.
The two-item comparison and copy operation uses the existing ItemDataRecord
and BattleCmdEntry layouts; no new opaque byte-offset accesses are needed.
The pool index uses the established contiguous fallback-plus-nine-pools
retail layout. Valid pool indexes are 0..9; larger indexes are outside the
established contract. Existing raw-ID lookup keeps the D_8009DE64 biased
address view, whose layout is already represented in inventory_slots.h.

This contiguous entry joins Inv_ItemActions.c: all six functions match
2780 code bytes, and the existing jump table matches all 56 bytes. Only
1044 bytes and one function are new. The canonical void prototype replaces
the menu caller's unspecified local declaration. Grouping is based on
adjacency and shared item state, not claimed original object metadata.

100000 ASan/UBSan model cases exercise both reserves, all established pool
indexes, negative and large capacity bonuses, valid and invalid tracked
indexes/IDs, overlapping full records, no-change early returns, and allocation
or lookup callbacks mutating active item pointers or snapshots. Tests compare
all record/command state and callback counts and arguments. The host harness
provides the retail contiguous pool/biased-ID backing layout and removes only
target ABI assertions; unrelated functions are discarded at link time.
Arbitrary partial overlaps and command-memory aliases are not covered.
Evidence: /tmp/pe-commit-ammo-comparison/ (check.py, check-unit.py, test.c).
The previous 306368 reload and 231424 dispatcher cases also pass against
the expanded unit (637792 host cases total). Retail jump-table entry 7 at
0x8001118C targets 0x800514D0, confirming that mode 7 never reads the buffer.

`make -j8 verify-clean` passes all 341 tests and source/debt/organization
gates. Main retains SHA-1 `452fb033f2eaa4b18aa20a5bca60b8125af3a37b`.
All 191 rebuilt overlays retain their retail SHA-1 values.

### Filtering records with tail capacity (0x8005CAEC)

`Inv_TransferToStorage` matches all 440 retail bytes with stock native GCC
2.7.2 and unmodified MASPSX. Despite the retained historical name, it selects
the Aya inventory, builds a filtered list using mask 0x3FE, clears selection
bits, and marks each filtered position whose item tailCount is less than
pad_00[1]. It returns the number of marked positions. The observed predicate
suggests available modification capacity, but no field rename or storage
transfer claim is made from that inference alone.

Separate inline bit clearing and a while loop with an explicit break retain
the original top-of-loop length reload and 40-byte stack frame. A for loop
with the same condition rotates differently in GCC 2.7.2. No pins, barriers,
gotos, instruction ASM, NOPs or extra compiler flags are added. Existing
ItemDataRecord and InventoryRuntime types cover the operation, including
the established biased D_8009DE64 lookup view. The canonical prototype
records the returned count. Adjacent entries remain unmatched and are not
wrapped into this C unit as assembler bodies.

194896 ASan/UBSan model cases cover every pair of the two compared byte
values, all 392 supported record IDs, filtered counts -1..64, negative/zero/
positive bit-word counts, and callbacks changing the active list, selection
bit pointer, or filtered length. Comparisons include the returned count,
complete record/list/bitset state, and callback arguments/counts. Production
code is tested unchanged except target ABI assertions and host backing for
the existing retail address aliases. A resolved filtered record must be
non-null, and callback-provided lengths/indexes must fit their backing arrays:
retail dereferences the result without a null guard. The tested maximum
filtered length is 64 and active length is 50; larger backing configurations
are not claimed. Evidence: /tmp/pe-filter-mod-capacity/ (check.py, test.c).

`make -j8 verify-clean` passes all 341 tests and source/debt/organization
gates. Main retains SHA-1 `452fb033f2eaa4b18aa20a5bca60b8125af3a37b`.
All 191 rebuilt overlays retain their retail SHA-1 values.

### Threshold-qualified equipment selection (0x8005C688)

`Inv_RebuildWithBonusSlots` matches all 1124 retail bytes with stock native
GCC 2.7.2 and unmodified MASPSX. It selects the Aya list and builds kind mask
0x1FE when D_8009D0CC is nonzero, otherwise 0x200. It clears the selection bits
and marks filtered positions whose three base-plus-signed-bonus values meet
the global first-stat threshold and the two argument thresholds. Item IDs
0x93 and 0x61 are excluded. If the count is nonzero, it scans all 128 equipment
records for a nonzero allocation marker, flag 0x10, and kind below 9 in the
first mode or exactly 9 in the second. Finding such a record negates the
count. D_8009D068 receives count != 0, including for negative counts.

The two scans use empty-body for loops with the full search predicate in
the condition. Stock GCC's normal passes then expand the first two scan
iterations exactly as retail does. Keeping the scan pointer separate from
the earlier resolved-item local gives the original allocation. No explicit
unrolling, pins, barriers, gotos, instruction ASM, NOPs, or new flags are
needed. A plain loop with break generated a shorter but nonmatching scan;
compiler changes and unroll flags were unnecessary.

The contiguous threshold and tail-capacity filters now form
Inv_EquipmentSelection.c, sharing record lookup and bit clearing. Both
entries match all 1564 bytes; only 1124 bytes and one function are new. The
former Inv_TransferToStorage.c is absorbed into this unit. The two filter
globals and the function signature have canonical inventory declarations.
This grouping follows shared state and adjacency, not recovered object names.

104736 ASan/UBSan model cases cover all supported 392 IDs, every signed
halfword value for the argument thresholds, both filter modes, excluded item
IDs, negative bonuses, no results, beginning/end/absent tagged records, and
callbacks mutating the selected list, bit pointer, filtered length, mode and
first-stat threshold. The previous 194896 tail-capacity cases also pass
against the combined production source (299632 total). Tests compare whole
record/list/bitset state, returned signed counts and callback arguments/counts.
Host aliases reproduce the existing retail backing layout, with only target
ABI assertions disabled. Filtered entries must resolve to valid records;
array lengths/indexes must fit their backing allocations. Tests use filtered
lengths through 64 and active lists of 50, not arbitrary larger allocations.
Evidence: /tmp/pe-rebuild-bonus/ (check.py, check-unit.py, test.c), plus the
previous /tmp/pe-filter-mod-capacity/test.c.

`make -j8 verify-clean` passes all 341 tests and source/debt/organization
gates. Main retains SHA-1 `452fb033f2eaa4b18aa20a5bca60b8125af3a37b`.
All 191 rebuilt overlays retain their retail SHA-1 values.

### Initializing a new-game inventory (0x8005CCA4)

`Inv_InitNewGameInventory` matches all 892 retail bytes with stock native
GCC 2.7.2 and unmodified MASPSX. It initializes seven stat-allocation values
from the growth tables, loads level-zero HP and capacity, unlocks the first
spell, clears the first three ammo pools, selects Aya and clamps capacity to
50. It clears 50 active slots backwards, adds the five initial item IDs,
then carries forward the first allocated flag-0x10 non-armor and armor
records. Their first matching references are removed from the 82-slot
special storage list before insertion. It sets equipped slots, menu context,
EXP and level, and initializes the menu background.

The storage address reuse exposes a missing aggregate relationship:
InventoryRuntime now includes the 12-byte gap after equipment, 100 normal
storage slots at offset 0x1098, and 82 special slots at 0x1160. Those bounds
also agree with the previously matched storage selection. Offset assertions
record this layout. AyaSaveState's placeholder one-element list is replaced
by its 50 signed slots, ending at equipment offset 0xAC. Its capacity is a
byte at 0x0C, with the remaining three bytes explicitly reserved. The level
table's old u16 field at 0x06 is split: byte 7 is the initial inventory
capacity. Other unknown fields retain neutral names. The initializer accesses
these real members directly, without new biased pointer arithmetic or aliases.

The growth-table API now consistently returns a data pointer rather than an
integer in its definition and an int pointer at its callers. Its consumers
use 32-bit threshold rows and, here, the initial low halfword. The level-table
API consistently returns AyaLevelStats*, including the HP consumer. Shared
inventory/menu declarations cover the initializer's calls. This is signature
and layout reconciliation; the existing functions must retain their bytes.

A write cursor for the seven allocations reproduces the initial register
order. The iteration variable is reused for the two selected record IDs
after the loops finish. Combined with the storage fields, this reproduces
all bytes without pins, barriers, gotos, instruction ASM, NOPs or new flags.

73728 ASan/UBSan cases cover every halfword HP value, every byte capacity,
clamping, absent/first/last tagged records, kinds including zero, storage
removal including slot 81, and callbacks changing capacity, active-list
pointers, and the armor candidate between insertions. The host harness uses
shared backing for the overlapping save/inventory/stat views, verifies their
physical offsets, and compares full state plus callback order and arguments.
Only target ABI assertions are disabled. Tables are valid, non-overlapping
backing objects; arbitrary table/runtime aliasing is not claimed. Mocked
insertion callbacks test this function's interaction order, not the complete
item-add implementation. Evidence: /tmp/pe-new-game-inventory/ (check.py,
test.c and build logs).

`make -j8 verify-clean` passes all 341 tests and source/debt/organization
gates. Main retains SHA-1 `452fb033f2eaa4b18aa20a5bca60b8125af3a37b`,
including every existing function affected by the shared-type corrections.
All 191 rebuilt overlays retain their retail SHA-1 values.

### Updating the special equipment selection (0x8005D020)

`Inv_MergeStorageToSlot` matches all 660 retail bytes with stock native
GCC 2.7.2 and unmodified MASPSX. The historical name is retained without
claiming that this is a storage merge: it selects Aya's list, removes the
first generated equipment record with itemId 0x61, adds raw item ID 0x93,
and selects the first kind-7 record as the tracked weapon. Removing tracked
armor preserves the capacity query, release, compaction and mode-3 update;
the final update clears battle equipment mode and uses mode 2.

The function shares `item/Inv_InventorySetup.c` with the adjacent new-game
initializer. All 1552 bytes of the combined object match. Initializing the
excluded-slot address before the search cursor and preserving the operand
order of the armor comparisons reproduce the register allocation without
pins, barriers, gotos, instruction ASM, NOPs or new compiler flags.

Two pointer-to-integer casts are explicitly added to the debt baseline.
The inlined search retains retail's excluded-index comparison with -1.
Unsigned address arithmetic represents that sentinel without forming a C
pointer before the list. This is target address-model debt, not portable
pointer arithmetic: unsigned long holds a pointer on the target and host
harness. The search otherwise stays within one valid list backing object.
It requires each visited ID to resolve to a non-null record, as retail does.
No new null check or upper-bound guard is invented in the removal path.

100000 ASan/UBSan model comparisons exercise capacities 0..50, base,
generated and ammo-pool records, missing and present matches, tracked armor,
and callbacks redirecting the list or changing its tracked selection.
Callbacks are mocked; these tests establish interaction order and arguments,
not the complete insertion/compaction implementations. Redirected removal
IDs remain within the equipment backing. Another 73728 existing new-game
cases pass against the combined source. Evidence is under
/tmp/pe-special-equipment/ (check-unit.py, test.c, test-newgame.c and logs).

`make -j8 verify-clean` passes all 341 tool tests and source/debt/organization
gates. Main retains SHA-1 `452fb033f2eaa4b18aa20a5bca60b8125af3a37b`.
All 191 rebuilt overlays retain their retail SHA-1 values.

### Inventory and menu command dispatcher (0x8005D2B4)

`Menu_InitBonusPointScreen` is a historical name for a dispatcher covering
commands 1100..1120, not solely a bonus-point screen initializer. All 1088
code bytes and the 84-byte compiler-generated jump table match retail with
stock native GCC 2.7.2 and unmodified MASPSX. It joins the adjacent inventory
initializers in `item/Inv_InventorySetup.c`; the combined 2640 code bytes
and table match exactly. The boot interpreter's fourth pointer argument is
retained in the shared prototype, although this dispatcher does not use it.

Commands count occupied slots or occurrences of an item (generated equipment
is compared by its base item ID), query or set capacity, query/change battle
stats, remove an active item or the first matching normal-storage reference,
and invoke menu, equipment and new-game setup operations. The ammo query
special-cases three pools indexed from D_8009D03C. Command 1117 sets the
existing game state's pending story day to one, increments save byte 0x0B
with saturation at 99, and calls the new-game initializer. Neutral names
remain for incompletely understood state; the historical function name is
not offered as semantic evidence.

A separate inlined capacity setter preserves the original argument copy.
The ammo-count branch shares its return variable with the inventory scan;
the saturating increment uses explicit if/else. No new pins, barriers,
address casts, aliases, gotos, instruction ASM or compiler flags are needed.
The capacity query uses the two existing, audited views of the same byte.
Its bonus queries are explicitly sequenced before each byte read. A host
test with a mutating query exposed unspecified operand evaluation order in
the initial draft; explicit sequencing fixes that without changing target
bytes. Shared subsystem declarations replace the new draft's local externs.

100000 ASan/UBSan comparisons cover all 21 commands plus defaults, capacities
0..50, signed slot IDs, equipment base IDs, ammo pools, present/absent item
searches, first-match storage removal, byte wrapping for negative capacity
inputs, and saturation for every value of save byte 0x0B. Callback traces
check order, arguments and results, including queries mutating the capacity
and redirecting the active list. Valid list backing and non-overflowing
ammo-base arithmetic are the tested domain. Callees are mocked, so their
implementations are not inferred from the dispatcher harness. The harness
extracts the dispatcher and its inline helpers unchanged from production;
only target ABI assertions are omitted for the host. The neighboring two
functions also pass their existing 173728 sanitizer cases with the expanded
TU. Evidence: /tmp/pe-bonus-dispatch/ (check-unit.py, test.c, tested.c and logs).

`make -j8 verify-clean` passes all 341 tool tests and source/debt/organization
gates. Main retains SHA-1 `452fb033f2eaa4b18aa20a5bca60b8125af3a37b`.
All 191 rebuilt overlays retain their retail SHA-1 values.

### Initializing save metadata and runtime defaults (0x8005D6F4)

`Save_InitMetadataState` matches all 588 retail bytes with stock native
GCC 2.7.2 and unmodified MASPSX, without new pins, barriers, gotos, aliases,
address casts, instruction ASM or flags. Separating the buffer fill from
its captured copy destination and using an ordinary destination/source copy
helper reproduces allocation and the first expanded string-copy iteration.

The function clears the complete 0x12E4-byte runtime save block. It initializes
the same second metadata window twice: both eight-byte fills, both table-4
lookups of entry 30 and both FF-terminated copies are retained. This is an
observed repetition, not an assumption that two different windows were
intended. It copies a third lookup to the prompt, initializes the inventory,
sets special-storage slot zero to raw ID 515, sets the saved/draw blend color
to 0x404040, clears four timer values, configures draw/menu defaults and
writes FF to the leading bytes of the two later display-text buffers.

SaveBytes12E4 now exposes the two existing 16-byte metadata windows rather
than only an opaque byte array; the remainder stays uninterpreted. The
shared SaveMetadataWindow type retains its size, and assertions cover the
whole block and second-window offset. Existing full-block assignments keep
the same type and size. AyaSaveState's offset 0x44 becomes blend_color,
corroborated by the initializer and Save_RestoreHeader's Draw_BlendColor
call. It replaces four reserved bytes without changing layout. The canonical
metadata globals and subsystem declarations are reused; no duplicate
address aliases are added.

100000 ASan/UBSan cases compare the complete overlapping save/Aya/inventory
backing, timer values, prompt contents and callback traces. They exercise
all terminating positions within a 16-byte metadata window, differing
strings across the repeated lookups, empty strings, final text sentinels and
callbacks redirecting the metadata cursor. The destination is captured
before lookup, as in retail. Strings must terminate within valid backing;
the unchecked retail copy is not claimed to be bounds-safe for invalid
input. Mock callbacks validate interaction ordering rather than the callees'
implementations. Another 273728 existing inventory/dispatcher cases pass
with the revised shared headers. Evidence: /tmp/pe-save-init/.

`make -j8 verify-clean` passes all 341 tool tests and source/debt/organization
gates. Main retains SHA-1 `452fb033f2eaa4b18aa20a5bca60b8125af3a37b`,
including the pre-existing full save-block copy and inventory functions.
All 191 rebuilt overlays retain their retail SHA-1 values.

### Recalculating Aya's runtime battle stats (0x8005218C)

`Inv_RecalcSlotStats` matches all 620 retail bytes with stock native GCC 2.7.2
and unmodified MASPSX. It computes saved maximum HP even without a player
entity; with a live core it updates maximum HP, applies signed-halfword
clamps to the two battle HP fields and an unsigned clamp to saved current
HP, then queries growth categories 1..6 to fill the remaining combat stats.
Capacity has no category offset and is passed to Inv_SetAyaSlotCount before
the entity's displayed level is refreshed from the save state.

AyaSaveState now exposes the seven contiguous allocation values as one
array at offset 0x28, replacing tentative individual labels. A signed
halfword cursor traverses that array using the permitted corresponding
signed/unsigned type view. This lets the compiler derive saved max HP from
the same aggregate address without out-of-bounds pointer arithmetic. The
bonus-point input consumer uses the shared array too.

AyaLevelStats offset 0x16 is a u16, not two independent byte fields: retail
loads the complete halfword into Combatant.statusStep3E. Other previously
unnamed table fields are named for their observed Combatant destinations,
including battleMaxAtk, battleAtbRate and battleAtbStep. These names record
the copy relationship, not a new claim about their broader gameplay meaning.
Shared declarations reconcile Stat_QueryLevelAndSubLevel's signed input and
pointer output with its definition. Layout assertions preserve all sizes
and relevant offsets.

Putting the maxHP assignment inside the first clamp condition reproduces
the retail value flow without pins, barriers, gotos, instruction ASM or new
flags. The function joins its adjacent Aya_DeriveStats/Battle_SyncEquipSlots
consumers in Aya_StatDerivation.c; all 836 bytes match. Existing constraints
and legacy pointer views in those consumers are retained. The audited
baseline drops one local extern, two unknown-field uses and one placeholder
filename; none of the matching crutch counts increases.

100000 ASan/UBSan model comparisons exercise every halfword base HP, signed
HP comparisons, both null exits, all seven stat categories, nonzero high
bytes in the recovered status-step field, and callbacks mutating modifiers,
future allocation inputs, the player pointer and saved level. The captured
combatant remains the destination when later callbacks change the global
player pointer. HP multiplication and level-offset addition are tested in
their non-overflowing domain; table records are valid, separate objects.
Callees are mocked and callback order/arguments plus full state are compared.
The harness extracts the new function unchanged from production and disables
only target ABI assertions for host compilation. Another 73728 new-game
cases pass with the revised shared header. Evidence: /tmp/pe-recalc-stats/.

`make -j8 verify-clean` passes all 341 tool tests and source/debt/organization
gates. Main retains SHA-1 `452fb033f2eaa4b18aa20a5bca60b8125af3a37b`,
including the existing bonus-point input and stat-derivation consumers.
All 191 rebuilt overlays retain their retail SHA-1 values.

### Packing equipped armor parameters (0x80051E64)

`Inv_BuildArmorList` matches all 808 retail code bytes and its 68-byte
compiler-generated jump table with stock native GCC 2.7.2 and unmodified
MASPSX. The historical name is retained: the operation packs three armor
parameters and effect flags rather than constructing a list. It selects
Aya, resolves the tracked armor slot, combines base and signed bonus stats,
and calls BattleCmd_LoadWeaponModifiers on both the present and absent paths.

The first two sums are capped at 999, with no invented lower clamp, and
stored modulo 1024. The third capped sum uses floor(sqrt(value * 3000))/10
below 85 (zero for negative inputs), otherwise (value * 249 / 208 + 402)/10.
The integer square-root loop is ordinary C and reproduces all instructions.
Effect codes use the low five bits of each tail byte. Codes 8..10 replace
the four-bit subgroup at bits 5..8, so the last such code wins; other known
codes accumulate flags. Missing armor clears the three parameter fields
while preserving the high reserved nibble.

BattleParameterWord supplies both a raw word and unsigned 10/10/8/4-bit
fields inside the existing eight-byte BattleAttributes layout. Field names
remain positional because their gameplay meanings have not been established.
The native object match verifies target bit ordering; host tests independently
compare the raw masks. Existing offset-based consumers retain their layout.
The previously scalar g_BattleEquipStateBlock is now consistently declared
as the full BattleAttributes object in its two users. Its existing second-word
view and matching constraints are retained. Shared declarations lower local
extern debt by two. No new pins, barriers, instruction ASM, aliases, address
casts, gotos or compiler flags are needed.

100000 ASan/UBSan model cases cover every signed-halfword third bonus,
negative and capped sums, the 84/85 boundary, every encoded effect byte,
empty/full valid tails, both exclusive-effect orders, absent armor and
callback mutations of the selected slot/output. The independent model uses
binary search for square root and raw integer masks rather than bitfields.
Full state and callback order/arguments are compared. Records and output
have separate backing, and tailCount is bounded by the eleven-byte tail;
unchecked retail behavior outside that domain is not claimed safe. Target
ABI assertions are disabled only for the host harness. Evidence:
/tmp/pe-armor-parameters/ (check.py, test.c and acceptance logs).

`make -j8 verify-clean` passes all 341 tool tests and source/debt/organization
gates. Main retains SHA-1 `452fb033f2eaa4b18aa20a5bca60b8125af3a37b`,
including the existing equip-list and battle-state initialization functions.
All 191 rebuilt overlays retain their retail SHA-1 values.

### Escape probability

`Battle_RollEscapeChance` is 744 exact code bytes plus its 20-byte switch
table, compiled with stock native GCC 2.7.2 and unmodified MASPSX. It uses
existing BattleEntity, EnemyCombatant and Combatant layouts, with no pins,
barriers, instruction ASM or added debt. Enemy hpAlive is a signed word;
player HP comparisons explicitly use signed halfwords. The probability is
signed eight-bit, including wraparound after multipliers. The player pointer
is reloaded after the metadata-window callback before incrementing attempts.

100000 ASan/UBSan model cases cover list filtering, signed ranks, escape
blocking, HP thresholds, all attempt values, probability wraparound, both
message windows and callback redirection of the player pointer. The model
uses explicit integer narrowing and compares return values, state and call
counts. Narrow comparison confirms both code and jump-table bytes against
retail. Local evidence: /tmp/pe-escape/ (check.py, test.c, acceptance logs).

Clean acceptance passes all 341 tool tests and the unchanged main SHA-1
`452fb033f2eaa4b18aa20a5bca60b8125af3a37b`. All 191 rebuilt overlays match
retail. The audited report credits 2479756 semantic code bytes and 10718
functions; debt counts remain unchanged.

### Battle target cycling

`Battle_CycleTarget` is 780 exact code bytes plus its 36-byte switch table.
Stock native GCC 2.7.2, G0 and stock MASPSX G8 with --expand-div reproduce
retail's signed remainder checks. There are no new pins, barriers, instruction
ASM or aliases. The existing BattleTarget layout explains the twelve-byte
stride and distance field, and EnemyCombatant supplies the entity-id byte.
The out-of-range flag is narrowed to a byte before being passed to the UI.

The audio handle uses the existing volatile-pointer view from the menu sound
routines: its test and subsequent command argument are separate reads. The
shared AKAO declaration uses the implementation's void-pointer parameter.
The target index uses its existing signed-byte alias at 0x8009CE44; the
common declaration emits GP-relative accesses and the existing linker alias
keeps it at the retail address, without allocating replacement storage.

100000 ASan/UBSan model cases compare state and complete callback traces for
all signed-byte modes, zero through eight targets, both input bits, range
checks and mutations by marking, fade, audio, animation and UI callbacks.
Valid indices and nonzero divisors after callbacks bound the tested domain.
Host-only headers disable target-layout assertions; production keeps them.
Narrow checks prove both code and switch-table bytes. Evidence:
/tmp/pe-cycle-target/ (check.py, test.c and acceptance logs).

`make -j8 verify-clean` passes 341 tool tests and the unchanged retail main
SHA-1 `452fb033f2eaa4b18aa20a5bca60b8125af3a37b`.
All 191 rebuilt overlays match retail; the audited report credits 2480536
semantic code bytes and 10719 functions. Debt counts remain unchanged.

### Target selection and action queue setup

`Battle_SetupEntityTarget` is 784 exact code bytes with stock native GCC 2.7.2
and unmodified MASPSX. It uses the existing BattleEntity, EnemyCombatant,
BattleAction and eight-byte BattleInitSlot structures. The four animation
bytes remain a single array. Three PE1_NOP_DEP calls retain retail load-delay
slots before stores to its nonzero GP offsets; these use the user's approved
NOP-only exception and increase nop_barriers from 142 to 145. No pins, empty
barriers, other CPU instruction ASM or toolchain changes are introduced.

The first scan deliberately retains the retail check of actor->entityFlags,
not target->entityFlags. With stable, separate backing this cannot select a
target in the branch entered for bit 0x40000000. Replacing it with the likely
intended candidate check would change behavior. The child-target path tests
parent, signed state byte and word-sized hpAlive. Queue entries retain the
signed-byte action index in a signed halfword. The effect callback is followed
by a fresh player-pointer load before marking the action committed.

100000 ASan/UBSan model cases cover all four routing paths, empty/nonempty
lists, null candidate cores, living/dead children, queue counts 0..15, all four
turn modes and callback redirection/state changes. Complete queue contents,
callback snapshots and global outputs are compared. The first-path model
uses the stable-flag invariant independently of the production scan. Inputs
use separate backing and valid queue capacity (initial count 0..29, at most
15 new entries); unchecked overflow behavior is not claimed safe. Production
NOP macros remain active in the host harness; only target-layout assertions
are disabled. Evidence: /tmp/pe-setup-target/ (check.py, test.c, acceptance logs).

`make -j8 verify-clean` passes all 341 tool tests and main's unchanged retail
SHA-1 `452fb033f2eaa4b18aa20a5bca60b8125af3a37b`.
All 191 rebuilt overlays match retail. The report credits 2481320 semantic
code bytes and 10720 functions; dirty files increase by one for the documented
NOP constraints, with all other debt categories unchanged.

### Battle item-menu input handling

`Battle_HandleItemMenu` is 1816 exact code bytes using stock native GCC 2.7.2
and unmodified MASPSX. Its three byte-sized local flags explain register
allocation without pins; a switch preserves targeting-mode dispatch. Inline
helpers express the repeated finish-action scan and audio command dispatch.
No new barriers, NOPs, instruction ASM, aliases or other debt are needed.

The existing BattleInitSlot array supplies the unsigned-halfword command
read at entry offset four. The first command gates scans for queued actions
1/2; command ranges 3..406 and 387..407 retain their distinct bounds. Shared
headers now declare the called interfaces. Battle_FillActionQueue's previous
void parameter list was incorrect: this caller passes a BattleTarget entry
and the retail callee consumes the pointer. Active-slot and menu-state bytes
use their existing signed aliases, including truncation of callback results.

100000 ASan/UBSan model cases compare return values, complete callback traces
and resulting player/global state. They cover empty/full valid queues,
command-range boundaries, input combinations, cancellation, target submission,
item-menu opening, turn advance, signed callback-result narrowing and callback
mutations of player pointers, buttons and active-slot state. Every reachable
callback is covered; the original redundant menu-state-2 undo branch remains
in C but is not asserted reachable with separate global backing. The model
uses direct range bounds and a lookup table for targeting mode, independently
of the production unsigned-range expressions and switch. Host-only headers
disable target-layout assertions. Evidence: /tmp/pe-handle-item-menu/
(check.py, test.c and acceptance logs).

`make -j8 verify-clean` passes 341 tool tests and main's unchanged retail SHA-1
`452fb033f2eaa4b18aa20a5bca60b8125af3a37b`.
All 191 rebuilt overlays match retail. The audited report credits 2483136
semantic code bytes and 10721 functions, with unchanged debt counts.

### Filling the battle action queue

`Battle_FillActionQueue` is 548 exact code bytes with stock native GCC 2.7.2
and unmodified MASPSX (--expand-div preserves retail's signed-remainder
checks). Existing BattleTarget and BattleInitSlot layouts account for all
accesses. C inline helpers separate writing a slot from resolving its index;
no pins, barriers, NOPs, instruction ASM or new debt are introduced.

Mode 0xC0 walks the null-terminated target list, independently of the target
count. Mode 0x40 chooses random targets for floor(3 * action-count / 2)
entries; its loop bound is reloaded after every random call. Other modes
append the selected actor and decrement the byte-sized remaining count,
including wraparound from zero. The random path explicitly captures the
queue index before rand(), then loads the target count and signed action
index afterwards. The queue count increment uses the current global value,
which can differ from the captured destination after a callback mutation.

100000 ASan/UBSan model cases compare all queue entries, callback-time queue
snapshots and resulting globals. They cover every mode and low-nibble count,
empty/full target lists, signed action indices, remaining-count wraparound,
0 and 22 random draws, plus callbacks that change the queue index, target
count, active action byte and player pointer. Tests use separate backing,
positive divisors, bounded null-terminated lists and queue destinations below
45. Target-layout assertions alone are disabled for the host. Evidence:
/tmp/pe-fill-action-queue/ (check.py, test.c and acceptance logs).

`make -j8 verify-clean` passes all 341 tool tests and main retains retail SHA-1
`452fb033f2eaa4b18aa20a5bca60b8125af3a37b`.
All 191 rebuilt overlays match retail. The audited report credits 2483684
semantic code bytes and 10722 functions, with unchanged debt counts.

### Recursive widget destruction

`MenuWidget_DestroyNodeRecursive` is 404 exact code bytes with stock native
GCC 2.7.2 and unmodified MASPSX. The existing MenuWidgetNode layout accounts
for the linked lists, four child pointers and mode-2 references at 0x78/0x7C.
The reciprocal links are named linkedPrevious/linkedNext, based on paired
writes in Menu_ConfigureScreen and Menu_CreateItemDetailView. These names
describe the observed topology, without claiming a navigation direction.
No pins, barriers, NOPs or instruction ASM are introduced.

The node is removed from the active list and prepended to the free list
before its mode-2 callback and recursive child destruction. The current-node
check happens after the children return. Remaining active nodes then lose
parent/child references to the removed node, and mode-2 nodes also lose the
two auxiliary references. Missing nodes are ignored; unlinking before
recursion also terminates cycles and repeated references without revisiting
a freed node. Globals use existing named aliases, and called interfaces are
declared in the shared menu header.

100000 ASan/UBSan cases compare against an independent model that maintains
the active order in an integer array. They check all node references, free
and active list heads, focus, and complete callback traces, including list
heads observed by the mode-2 callback. Cases include empty/full lists, absent
nodes, self-links, cycles, shared children and callbacks that change the
removed node's child, parent and current focus. Host-only headers disable
target-layout assertions. Evidence: /tmp/pe-destroy-widget/ (check.py,
test.c and acceptance logs).

`make -j8 verify-clean` passes 341 tool tests and main retains retail SHA-1
`452fb033f2eaa4b18aa20a5bca60b8125af3a37b`.
All 191 rebuilt overlays match retail. The unknown-field usage baseline
falls from 252 to 248; compiler-crutch counts do not increase.
The audited report credits 2484088 semantic code bytes and 10723 functions;
the dirty-file count falls from 2280 to 2279.

### Pad-driven entity facing

`Scene_UpdateEntityFacingFromPad` is 380 exact code bytes with stock native
GCC 2.7.2 and unmodified MASPSX. It uses the existing BattleEntity facingAngle
and player Combatant stateFlags fields. Shared headers declare its pad and
camera inputs and Entity_SetAction interface; no local externs or pointer
casts are added.

Analog packets (header high nibble 7) use Gte_Atan2 on the centered left-stick
coordinates, subtract a quarter turn, conditionally add a full turn and
apply the analog camera angle. Digital input preserves down/up/left/right
precedence, including simultaneous directions, and retains the previous
signed heading when no direction is pressed. The player-state adjustment
and optional reflection happen after applying the relevant camera angle.

Two explicitly accounted empty compiler barriers retain retail scheduling.
The first preserves the adjusted angle as the basis for the negative-angle
correction; removing it changes one instruction. The memory barrier after
storing the masked heading preserves the subsequent mode and heading reads.
No pins, NOPs or CPU instruction ASM are used. The main barrier baseline
increases from 902 to 904 (1050 including overlays).

262144 ASan/UBSan model cases cover every pair of analog input bytes, all
65536 prior heading bit patterns with no digital direction, simultaneous
button combinations, both camera offsets, wraparound, null/non-null player
pointers, player-state orientation bits and reflection. The independent
model uses an ordered direction table and arithmetic quarter-turn selection.
Entity_SetAction and Gte_Atan2 stubs also mutate relevant state to check that
reads occur after the appropriate callback. Call counts and analog arguments
are checked. Only target-layout assertions are disabled for the host.
Evidence: /tmp/pe-pad-facing/ (check.py, test.c and acceptance logs).

`make -j8 verify-clean` passes 341 tests and the unchanged main retail SHA-1
`452fb033f2eaa4b18aa20a5bca60b8125af3a37b`. After spelling the two barriers
explicitly so the debt scanner counts both, `make -j8 verify` and the narrow
byte comparison pass again. All 191 rebuilt overlays match retail.
The audited report credits 2484468 semantic code bytes and 10724 functions.
The dirty-file count is 2280; pins, NOPs and instruction-ASM counts are unchanged.

### Scene asset unload callbacks and directory layout

`Asset_UnloadTableEntries` is 484 exact code bytes with stock native GCC
2.7.2, the standard -fno-force-mem option and unmodified MASPSX. The default
flag profile differs by one commutative pointer-add operand order. No pins,
empty barriers, NOPs or instruction ASM are introduced.

The shared scene_assets.h header identifies the handler's unload callback
at 0x18, 12-byte records with a handler byte at offset 7, and the directory
word containing a 22-bit blob-relative byte offset and 10-bit record count.
Pe1GameState now names its loaded_scene_assets pointer at 0x18C, preserving
the prior layout. This resolves to the existing g_LoadedSceneAssetBlock
address (0x800B0E64), also used by Boot_InitMemoryLayout and Asset_Find08w.
The 104-entry object table uses its existing D_800E1044 name, already read
by func_800CE49C; this routine clears entries 30..103.

When the scene-loaded flag is set, callbacks run for handler slots 0..7,
then slot 85, then eligible IDs 8..84 encountered in the scene records.
The loaded blob is captured after the first nine callback opportunities.
Each record iteration reloads its count, while its original record base and
directory pointers remain captured across callbacks. Handler-table loads
also remain fresh. The flag is cleared after callbacks. Independently of
the flag, slots 8..84 of the current handler table and object slots 30..103
are cleared; the other slots are retained.

50000 ASan/UBSan model cases compare callback order and callback-time state
snapshots as well as final flags, complete tables, blobs and return values.
They cover absent handlers/callbacks, invalid record IDs, counts 0..16,
25 callbacks, table and blob swaps, callback changes to directory counts
and offsets, and retained table ranges. The model uses integer table IDs
and array indices independently of the production pointer traversal. Host
fixtures use allocated serialized blobs; only target-layout assertions are
disabled. Evidence: /tmp/pe-unload-assets/ (check.py, test.c and logs).

Two blob-relative byte-address calculations are recorded in the existing
byte_pointer_arithmetic baseline (main: 328 -> 330); these decode serialized
offsets rather than bypassing the reconstructed record/handler layouts.
`make -j8 verify-clean` passes all 341 tests and main retains retail SHA-1
`452fb033f2eaa4b18aa20a5bca60b8125af3a37b`.
All 191 rebuilt overlays match their retail SHA-1.
The audited report credits 2484952 semantic code bytes and 10725 functions.
Compiler-crutch counts are unchanged; the two serialized-offset operations
increase the dirty-file count to 2281.

### Stat-slot input and list navigation widgets

`Menu_StatSlotInputHandler` (396 bytes) and `Menu_StepListNavigate` (384 bytes)
are exact with stock native GCC 2.7.2 and unmodified MASPSX. Both are semantic
C without pins, barriers, NOPs, instruction ASM or new debt. The former uses
the existing g_MenuItemUseMode alias and MenuWidgetNode layout. Its choice of
176/162/156 as a target X coordinate is expressed directly as a conditional
expression, keeping the shared offset across subsequent callback calls.

The stat handler always obtains child zero first. Input bit 0x4000 takes
precedence over 0x10000 and 0x40: it resets/selects the stat list and aligns
three panels with one captured displacement. Confirmation opens the spend
dialog at grid index + 5; cancellation invokes the existing cleanup callback.
Shared menu declarations now describe the called interfaces. The dialog's
first parameter is corrected from an integer to MenuWidgetNode *, matching
its use as the parent argument to MenuWidget_CreateSimpleNode.

The mode-3 navigation widget has a list pointer at 0x34, where ordinary grid
widgets store a width. MenuWidgetListNavigation provides this typed view and
asserts the list/flags offsets. Draw_SwapPrimBuffers's retail constructor
stores the managed node at 0x34 and copies its layout flags to 0x40; the new
navigation function consumes precisely those fields. The typed view avoids
routing a host pointer through the grid_width integer.

Navigation prioritizes 0x1004 over 0x4008, waits while scroll_adjust is nonzero,
and applies the original lower-bound/upper-bound clamp order. The managed
list's 0x40 value supplies signed half-speed, rounded toward zero. Sound is
played only when the scroll position changes. Navigation keys are handled
even without movement; otherwise flag 0x40 controls input propagation.

100000 stat-handler ASan/UBSan cases compare complete callback traces,
callback-time field snapshots, cursor state, positions and dialog indices.
They include all three-action priority combinations, all target X choices
and callbacks changing the mode, popup and panel position. A separate 262144
navigation-case model covers every low-16 input mask, preexisting animation,
negative/positive steps and speeds, clamp ordering (including inverted
limits), return values and sound callbacks changing state. The navigation
model uses wider arithmetic and explicit signed half-speed rounding; the
stat model uses a panel array instead of repeated calls. Only target-layout
assertions are disabled for the host. Evidence: /tmp/pe-stat-slot/ and
/tmp/pe-list-nav/ (check.py, test.c and acceptance logs).

`make -j8 verify-clean` passes all 341 tests and main retains retail SHA-1
`452fb033f2eaa4b18aa20a5bca60b8125af3a37b`.
All 191 rebuilt overlays match their retail SHA-1.
The audited report credits 2485732 semantic code bytes and 10727 functions
(70.01% of code). All debt counters and the 2281 dirty-file count are unchanged.

### Constructing a list-navigation widget

`Draw_SwapPrimBuffers` is 392 exact bytes with stock native GCC 2.7.2 and
unmodified MASPSX, without pins, barriers, NOPs or instruction ASM. Despite
its historical name, it allocates a mode-3 list-navigation widget. Inline
helpers express the active-list owner search and free-list allocation.
The existing MenuWidgetListNavigation view describes its managed-list pointer
and copied flags; untouched pool fields retain their previous contents.

The first active owner containing the list receives the new widget in its
first empty child slot. A full owner calls diagnostic 11 and continues;
an empty free list calls diagnostic 10 before the original null access.
The test stops at diagnostic 10, without claiming recovery is supported.
Diagnostic 16 is retained but unreachable after a valid allocation.

5000 cases execute the compiled MIPS instructions in Unicorn and compare all
node bytes, list heads and diagnostic-time snapshots against an independent
array model. Cases include absent owners, all four free child slots, full
owners, empty pools, arbitrary retained data, and diagnostic callbacks that
change the active head and source flags. This uses the actual 32-bit layouts,
including the overlapping navigation/grid views, without host pointer-size
adaptations. Evidence: /tmp/pe-create-nav/ (check.py, test.py and logs).

The shared constructor prototype now takes MenuWidgetNode *. Its existing
Draw_SetPrimCallback caller explicitly supplies that pointer; its object text
is unchanged. BoundsCheck_AssertStub now accepts the diagnostic code already
passed by callers. The field at 0x30 is named draw: Draw_PushPrimToList invokes
it with the widget after applying its text-cursor coordinates, and menu
constructors populate it with their rendering callbacks. This reduces the
unknown-field baseline from 248 to 231, including the new constructor's
still-unresolved field_28 initialization. Compiler-crutch counts do not rise.

`make -j8 verify-clean` passes 341 tests and main retains retail SHA-1
`452fb033f2eaa4b18aa20a5bca60b8125af3a37b`. All 191 rebuilt overlays match.
The audited report credits 2486124 semantic code bytes and 10728 functions
(70.02% of code). The retained unknown initialization adds one dirty file
(2282 total); pins, barriers, NOPs and instruction-ASM counts are unchanged.

### Drawing list-navigation indicators

`Draw_FlushFrontBuffer` is 556 exact bytes with stock native GCC 2.7.2 and
unmodified MASPSX. Its historical name describes neither its input nor its
behavior: it draws the mode-3 list navigator's blinking highlight, background
and up/down scroll indicators. The existing widget and navigation layouts
supply every field. Inline helpers express owner lookup, relative cursor
movement and cursor-stack restoration; no pins, barriers, NOPs, instruction
ASM or new debt are needed.

The owner determines the color selection. The routine saves the cursor,
positions the indicators beside the managed list, optionally draws a blinking
highlight for the focused navigator, draws its background, and emits arrows
according to the list's scroll position. It restores the cursor from the
current stack pointer after callbacks. Overflow and underflow invoke the
existing diagnostic stub. A null managed list is ignored; a non-null list
requires an active owner, as retail dereferences the lookup result unchecked.

5000 cases execute the generated MIPS code in Unicorn and compare complete
callback traces, callback-time memory snapshots, and final widget, drawing
state and cursor-stack memory against an array model. They cover null lists,
each owner child slot, focus and both blink phases, color choices, stack
bounds, both arrow conditions, and callbacks changing cursor coordinates,
stack depth, drawing height, managed-list pointer and scroll bounds. The
original captured list remains in use when a callback changes the navigator's
list pointer. Tests use the real target layouts and bounded signed arithmetic.
Evidence: /tmp/pe-draw-nav/ (check.py, test.py and acceptance logs).

`make -j8 verify-clean` passes 341 tests and main retains retail SHA-1
`452fb033f2eaa4b18aa20a5bca60b8125af3a37b`. All 191 rebuilt overlays match.
The audited report credits 2486680 semantic code bytes and 10729 functions
(70.04% of code). All debt counters, including 2282 dirty files, are unchanged.

### Dispatching queued field events to actor tasks

`Scene_UpdateEntityList` is 468 exact bytes with stock native GCC 2.7.2 and
unmodified MASPSX, without pins, barriers, NOPs or instruction ASM. It consumes
the field-event queue and prepends new tasks to actors' third task lists.
The existing FieldActor and TaskNode layouts describe the script cursor,
actor identifiers, task flags, payload and reciprocal task links.

FieldSfxQueueEntry now names its type/subtype, task argument and task value.
Its actor selector is enqueued as a full word but dispatched using only the
low byte, represented by a named union. Layout assertions retain the 12-byte
stride and offsets 4/8. The producer uses the word view and the consumer uses
the byte view. This lowers the unknown-field baseline from 231 to 227 even
with the new use of the still-unresolved TaskNode.field_0c slot.

A nonzero selector stops at the first matching field_sfx_id, including when
that actor has no script. A zero selector visits all actors matching the
16-bit type filter and byte subtype, provided their script cursor is nonzero.
The allocator runs before event arguments and the previous task-list head are
read. The queue count and actor links remain fresh across allocator calls;
the count is cleared after dispatch. Allocation success is an original
precondition, not a newly added recovery path.

5000 cases execute generated MIPS instructions in Unicorn against an
independent byte-addressed model. They compare allocator arguments, complete
callback-time memory snapshots and final queue, actor, task and global state.
Cases cover empty queues/lists, both filters, high selector-word bits,
16-bit type IDs outside the actor-byte range, duplicate IDs, missing scripts,
empty/nonempty task chains and callback mutations of parameters, queue count,
actor-list links and task-list heads. Fixtures keep queue counts within the
28-entry capacity and allocate distinct valid task nodes. Target layouts are
used directly, without host pointer-size substitutions.
Evidence: /tmp/pe-dispatch-sfx/ (check.py, test.py and acceptance logs).

`make -j8 verify-clean` passes 341 tests and main retains retail SHA-1
`452fb033f2eaa4b18aa20a5bca60b8125af3a37b`. All 191 rebuilt overlays match.
The audited report credits 2487148 semantic code bytes and 10730 functions
(70.05% of code). Compiler-crutch counts and 2282 dirty files are unchanged.

### Advancing geometry animations

`Scene_CheckBattleFlag` is 664 exact bytes with stock native GCC 2.7.2,
-fno-force-mem and unmodified MASPSX (--expand-div). Despite its historical
name, it advances room geometry animations. No pins, barriers, NOPs or CPU
instruction ASM are needed. The permuter reached no improvement over score
10 before being stopped; source shaping produced the final score zero.

GeomAnimationControl provides a 16-byte view of the existing GeomCtrlEntry:
a flag/count word, low-byte group plus signed 24-bit position with eight
fractional bits, signed step, unsigned 16-bit elapsed time and relative slot
offset. Each two-byte slot contains a render-entry index and signed duration.
Compile-time assertions check sizes and non-bitfield offsets. The common
integer typedefs now replace geom_state.h's duplicate primitive definitions.

An indexed control loop avoids an extra induction pointer. Keeping the
narrowed elapsed value in a local preserves the original 16-bit wrap before
comparison. The slot lookup uses subtraction of a negated frame index; this
is equivalent within the valid slot array and preserves retail's commutative
address-add operand order. Ordinary addition differs by exactly one word.
The signed 24-bit bitfield expresses truncation without left-shifting a
negative C value. Its representation is intentionally target-ABI-dependent.

The function gates updates on game flags and group, clears every animation
slot's render-selection bit, selects the current slot and advances its timer.
Negative durations are cleared and stop this entire update immediately.
Forward/reverse stepping supports wrapping or resetting and preserves retail's
endpoint formulas. Three byte-address additions decode serialized relative
control-table, render-table and slot-array offsets; these are recorded in the
byte_pointer_arithmetic baseline (330 -> 333).

10000 MIPS/model cases compare the complete serialized state and return value.
They cover both gates, inactive flags, group filtering, zero control count,
duplicate render indices, negative/zero/positive durations, 16-bit timer wrap,
signed steps, interior advancement and forward/reverse wrap/reset paths.
Fixtures use valid initial slot indices, nonzero lengths 1..8 and bounded
serialized arrays. The independent model uses integer offsets, explicit
24-bit truncation and mathematical floor division. No host layout adaptation
is involved. Evidence: /tmp/pe-geom-animation/ (check.py, score.py, test.py and
acceptance logs).

`make -j8 verify-clean` passes 341 tests and main retains retail SHA-1
`452fb033f2eaa4b18aa20a5bca60b8125af3a37b`. All 191 rebuilt overlays match.
The audited report credits 2487812 semantic code bytes and 10731 functions
(70.07% of code). Compiler-crutch counts are unchanged; the three relative
address calculations raise the dirty-file count to 2283.

### Texture scrolling and camera-relative parallax

`Scene_IsBattleMode` is 504 exact bytes with stock native GCC 2.7.2 and
unmodified MASPSX (--expand-div), without pins, barriers, NOPs or instruction
ASM. It updates texture coordinates and camera-relative parallax despite its
historical name. GeomScrollEntry supplies a 56-byte view of GeomEntry with
signed coordinates, unsigned moduli, signed velocities and fraction unions:
fractions are read as low bytes and written as whole halfwords.

GeomScrollState describes the observed prefix at 0x800BCF88: flags followed
by coordinates at 0x800BCF8C, saved coordinates at 0x800BCF90/92 and the origin
pair at 0x800BD028/2A. Script dispatch also writes that origin pair. The type
ends at the last observed field; its size does not claim the full extent of
the surrounding state. Existing absolute symbols retain their overlapping
views, with no new aliases. A scalar flags declaration differed by 13 words;
the actual containing-record view reproduces address materialization exactly.

Flag 4 applies signed fixed-point scrolling and signed remainders. Flag 8
then overrides it with parallax. The final parallax sum uses unsigned word
arithmetic before conversion to target signed int, preserving MIPS wrapping
without overflowing a signed C addition. Camera flag 0x80 triggers saving the
current coordinates and is cleared independently of the geometry count.
Both volatile header-pointer reads are retained: the first supplies count
and offset, while the second supplies the record-array base.

10000 MIPS/model cases compare complete geometry buffers, camera state and
relevant globals. They include empty arrays, both gate bits, all update-mode
combinations, negative signed remainders, full signed-halfword coordinate and
velocity boundaries, 32-bit parallax overflow, clearing fraction high bytes,
camera save/retain paths, and a pointer change between the two volatile reads.
The independent model uses byte offsets, mathematical fixed-point operations,
explicit word wrapping and signed remainder calculation. Scroll divisors are
positive as required by the original routine. Evidence: /tmp/pe-geom-scroll/
(check.py, test.py and acceptance logs).

One byte-relative addition decodes the serialized render-table offset. It is
recorded in the byte_pointer_arithmetic baseline (333 -> 334), rather than
presented as an opaque field access or compiler constraint.

`make -j8 verify-clean` passes 341 tests and main retains retail SHA-1
`452fb033f2eaa4b18aa20a5bca60b8125af3a37b`. All 191 rebuilt overlays match.
The audited report credits 2488316 semantic code bytes and 10732 functions
(70.08% of code). Compiler-crutch counts are unchanged; the serialized-offset
calculation raises the dirty-file count to 2284.

### Camera transition update (2026-09-22)

`Scene_IsNotBattleMode` is 824 exact retail bytes with stock native GCC 2.7.2
and unmodified MASPSX (`--expand-div`). No pins, barriers, inline assembly,
new aliases or compiler changes are needed. Its existing name is misleading:
the routine advances a linear or cosine camera transition, centers the selected
viewport and updates the transition timer and completion flags.

The viewport records have a 52-byte stride and halfword dimensions at offsets
40 and 42, distinct from the 56-byte geometry entries. Their shared type records
only these observed fields. Existing scalar aliases retain the camera state's
original addressing. Separate unsigned word temporaries for the centered
coordinates preserve the signed loads before the final halfword stores; direct
assignment otherwise produces two `lhu` instructions instead of retail `lh`.

10000 independent MIPS/model comparisons cover gate bits, all mode values,
linear/cosine interpolation, signed halfword boundaries, timer wrap, completion
flags and negative viewport dimensions. Callback probes change the start
coordinates, timer and duration during `rcos`: the code reloads the starts and
timer but keeps its captured duration. A hook also swaps the geometry pointer
between its two volatile reads, verifying that the first supplies the offset
and the second the base. Nonzero signed durations respect the retail division
precondition. Evidence: `/tmp/pe-camera-transition/check.py` and `test.py`.

One byte-relative addition decodes the serialized viewport-table offset and
raises the main byte_pointer_arithmetic baseline from 334 to 335. No compiler
crutch counts increase.

Clean acceptance passes 341 tests and preserves main retail SHA-1
`452fb033f2eaa4b18aa20a5bca60b8125af3a37b`. All 191 rebuilt overlays match.
The audited report credits 2489140 semantic code bytes and 10733 functions
(70.10% of code). Dirty files increase from 2284 to 2285 due to the serialized
offset calculation; all compiler-crutch totals remain unchanged.

### Projected camera-transition target (2026-09-22)

`Render_UpdateScrollPosition` is 608 exact retail bytes with stock native
GCC 2.7.2 and unmodified MASPSX. It captures the current camera coordinates,
projects the signed high halves of a 16.16 position through the GTE, centers
and clamps the projected point within a selected viewport, and initializes
transition target, duration, elapsed time and mode flags. Duration -1 defaults
to 30; mode -1 preserves the interpolation bit. The disabled path returns -21.
Shared declarations now record the actual int return type at both callers.

The recovered camera prefix has start/target pairs, elapsed/duration and a
matrix-word pointer at state offset 0x1C. The existing 52-byte viewport gains
signed min/max bounds at 44/46/48/50. Structure extents and preexisting aliases
are preserved. Matrix words are transferred in the retail 2/3/2/1 load groups;
each GTE transfer/command is an individual hardware macro. All ordinary CPU
loads, shifts, clamping, flag handling and stores are C.

Matching constraints: nine register pins, six empty barriers and two explicit
PE1_NOP hazard slots. One pinned center input is volatile unsigned short.
Without volatility, the generated function differs only in its stack
adjustments (16 bytes instead of 24). This is compiler debt, not evidence that
the original author used that volatile type. No invented padding or oversized
buffer is used. Native GCC frame-layout source inspection and the permuter
isolated this constraint; the successful source then passed the raw 608-byte
comparison. The initial parallel permuter harness reused a temporary filename
on macOS; that run was discarded. A corrected harness creates a unique
temporary directory per compilation and reached score 0 after 685 trials.

5000 independent MIPS/model cases pass for both retail and candidate. COP2
instructions are intercepted: the test checks all center/matrix registers
and the six meaningful input-vector bytes, supplies a controlled packed
projection result, then checks all camera state and relevant global buffers.
Coverage includes both gate paths, signed coordinates/dimensions, clamping
(including inverted bounds), arbitrary/default durations and mode flag masks.
This tests the surrounding CPU algorithm and GTE interface, not GTE hardware
arithmetic. The target and candidate issue identical GTE instruction bytes.
Evidence: /tmp/pe-camera-target/{check.py,test.py,permuter-clean.log}.

Debt records pins 1032 -> 1041, barriers 904 -> 910, NOPs 145 -> 147 and
byte-relative table addressing 335 -> 336 in main. Shared declarations remove
two file-local externs (3660 -> 3658). ASM-body and directive counts do not grow.

Clean verification and the final verification both pass 341 tests. Main keeps
retail SHA-1 `452fb033f2eaa4b18aa20a5bca60b8125af3a37b`; all 191 rebuilt overlays
match. The audited report credits 2489748 semantic code bytes and 10734
functions (70.12% of code), with 2286 dirty files.

### Object lighting matrices (2026-09-22)

`Render_InitRoomPrimState` is 468 exact retail bytes with stock native GCC
2.7.2 and unmodified MASPSX. Despite its existing name, it prepares two
opposite Y-direction lights and uploads their color matrix to GTE control
registers 16..20. Base shade at object offset 0x88 is added to the unsigned
positive/negative Y brightness fields at 0x8A/0x89, clamped to 0..255, scaled
by 16 and multiplied by captured RGB bytes with signed division by 256.
The third light direction and color column are cleared.

RenderObjectEntity now names the two brightness bytes without changing its
layout. RenderLightingMatrix combines the SDK MATRIX arithmetic view with
its eight-word storage view; the globals are spaced by 32 bytes. Existing
absolute halfword symbols retain their overlap with these matrices. The GTE
reads five packed words, including the preexisting alignment halfword after
the nine coefficients. Padding and translation fields are not initialized
by this routine and are preserved. The existing caller uses the same shared
matrix declaration and the actual int-returning function prototype.

Four register pins bind only the GTE transfer address and scratch values.
Four empty barriers retain the complete signed result before its halfword
store, materialize the matrix address after overlapping alias stores, and
preserve the two/three load groups. Each CTC2 is a separate existing GTE macro;
there are no ordinary CPU instruction bodies, new NOPs, directives or compiler
changes. Keeping the last color result in a short instead of int reduced the
three scheduling differences to one but selected a logical shift. The explicit
result barrier retains the full signed computation and matches all bytes.

10000 independent MIPS/model cases pass for both retail and compiled C.
The tests compare both complete 32-byte matrices, the unchanged input object
and RGB globals, and the ordered five GTE control-register writes. Cases cover
zero, unsaturated and saturated brightness, threshold/end-point input bytes,
random RGB values and nonzero original padding/translations. COP2 transfers
are observed through hooks; no GTE arithmetic command needs emulation here.
Evidence: /tmp/pe-lighting-init/{check.py,test.py,base.bin,target.bin}.

Main debt records four added pins (1041 -> 1045), four barriers (910 -> 914)
and one removed file-local extern (3658 -> 3657). All other categories remain
unchanged, including ASM bodies, directives, NOPs and aliases.

Clean acceptance passes 341 tests and preserves main retail SHA-1
`452fb033f2eaa4b18aa20a5bca60b8125af3a37b`. All 191 rebuilt overlays match.
The audited report credits 2490216 semantic code bytes and 10735 functions
(70.13% of code), with 2287 dirty files.

### Camera orientation basis (2026-09-22)

`Render_DrawSprite` is 632 exact retail bytes with stock native GCC 2.7.2
and unmodified MASPSX. The existing name is misleading: it builds a camera
orientation matrix. Input-mode bits select one of two yaw values. When the
active battle entity exists and mode bit 0x10 is set, its core state flags
add a quadrant offset and a half-turn. The angle is masked to 12 bits before
both trigonometric calls. The shared BattleEntity/Combatant types describe
the pointer chain; no new partial actor type or alias is introduced.

The function builds a Y rotation, transforms the forward axis using MVMVA,
and uses two OP commands to obtain right = up cross forward and corrected
up = forward cross right. These vectors become the output matrix columns;
translation is zeroed and its alignment halfword is preserved. GteMatrixStorage
provides arithmetic and packed-word views of the full 32-byte SDK matrix.
Every GTE transfer/command is wrapped individually; CPU-side work is C.

The 128-byte frame follows directly from MATRIX at +0x10, SVECTOR at +0x30,
four VECTOR objects at +0x38/+0x48/+0x58/+0x68, argument space and saved s0/ra.
There is no invented padding. Twelve register pins are confined to GTE
transfer addresses and scratch words. Eleven empty barriers preserve memory
visibility, load grouping and address scheduling; six PE1_NOP slots cover
COP2 transfer hazards. All constraints are explicit in the production source,
rather than hidden behind repeated CPU-operation macros.

5000 MIPS/GTE model cases pass for retail and compiled C. Trigonometric
callbacks return controlled values in the ordinary fixed-point range, clobber
caller-saved registers and optionally mutate yaw/mode globals. Both calls must
still receive the captured angle. The GTE model implements the relevant MVMVA
and OP fixed-point arithmetic for these bounded inputs; expected output uses
an independent closed-form yaw basis, including integer normalization error.
Checks cover both yaw sources, null/present actors, quadrant adjustment,
command sequence, complete output matrix including preserved padding, input
globals and s0/SP restoration. This is not a general GTE emulator or a test
of the existing trigonometric implementations. Evidence:
/tmp/pe-camera-basis/{check.py,test.py,base.bin,target.bin}.

Main debt increases by 12 pins (1045 -> 1057), 11 barriers (914 -> 925) and
six NOPs (147 -> 153). All other categories, including ordinary ASM bodies,
directives, aliases and raw pointer arithmetic, remain unchanged.

Clean acceptance passes 341 tests and preserves main retail SHA-1
`452fb033f2eaa4b18aa20a5bca60b8125af3a37b`. All 191 rebuilt overlays match.
The audited report credits 2490848 semantic code bytes and 10736 functions
(70.15% of code).

### Camera projection initialization (2026-09-22)

`Render_PrepareFrame` matches all 332 retail bytes with stock native GCC
2.7.2 and unmodified MASPSX. It initializes the camera context, loads geometry,
returns -2 if that load fails, sets the projection distance, resets objects,
and projects the active actor's integer world coordinates (or zero when no
actor exists). The packed screen coordinates are saved at 0x800BCFB4.
The existing BattleEntity position fields supply the signed high halfwords.

Eight register pins, two empty barriers and two explicit PE1_NOP slots remain;
every COP2 operation uses an individual GTE macro. No CPU instruction assembly,
compiler changes or new aliases are introduced. A narrow volatile center input
retains GCC's retail 48-byte stack frame; this is compiler debt, not a recovered
original type. The local aggregate holds the eight-byte vector, four-byte
loader output and four-byte projection output, with no invented padding.
Four provisional pins and eight provisional barriers were removed while
preserving the match. The two existing callers now use the shared int-returning
prototype. Main debt grows by eight pins, two barriers and two NOPs.

5000 MIPS/model cases pass independently for both retail and compiled C.
Checks cover load failure, null/present actors, signed-coordinate extremes,
ordered callbacks, mutations of projection distance and actor state during
callbacks, caller-register clobbers, all ten GTE control writes, input vector,
controlled projected output, return value and preserved s0/SP. COP2 transfers
are hooked and RTPS supplies a controlled result; this verifies the CPU logic
and GTE interface, not the hardware projection arithmetic. Evidence:
`/tmp/pe-prepare-frame/{check.py,test.py,base.bin,target.bin}`.

Clean verification passes 341 tests and preserves main retail SHA-1
`452fb033f2eaa4b18aa20a5bca60b8125af3a37b`.
All 191 rebuilt overlay binaries also retain their retail SHA-1.
The audited report credits 2491180 semantic code bytes and 10737 functions
(70.16% of code). Total debt is 1291 pins, 1073 barriers and 155 NOPs;
ordinary ASM bodies and directives remain unchanged.

### Room tile packet construction (2026-09-22)

`Geo_LoadMeshEntry` matches all 616 retail bytes with stock native GCC 2.7.2,
unmodified MASPSX and -fno-strength-reduce -fno-expensive-optimizations.
It builds two frame buffers of 16-byte textured tile sprites, followed by two
buffers of eight-byte texture-page packets. The returned end pointer advances
by 48 bytes per tile. RGB is 128; low 24-bit packet addresses are preserved.
CLUT and texture-page calls use packed descriptors; palette tracking preserves
the original signed comparison against the initial palette byte plus 479.
The descriptor word is reloaded after GetClut. Zero tiles produce no callbacks
and leave the palette byte unchanged.

Shared packet and descriptor types have checked sizes and field offsets.
GeomEntry+0x34 now names the texture-page pointer and exposes its four-byte
storage view. The store through that byte view retains GCC's original alias
scheduling; direct assignment to the pointer member changes twelve words.
This source constraint remains explicit rather than being called padding or
hidden in a macro. Two byte-relative additions decode serialized array offsets.
The background documentation now distinguishes sprite/page packet arrays and
frame buffering; this routine does not set the geometry entry's drawable flag.

Matching constraints are one palette-input pin and one empty memory-operand
barrier on disp_x. There are no instruction ASM bodies, new NOPs, aliases or
compiler modifications. Removing the pin changes 19 words; removing the barrier
from the final direct-Y form changes 13. The relational loop guard i<count,
with i initialized to zero, leaves a comparison USE pseudo in GCC's RTL; its
reload slot accounts for the retail 104-byte frame. A direct count!=0 guard
instead generates a 96-byte frame. No artificial local or padding was added.

5000 independent MIPS/model cases pass for both retail and compiled C, comparing
complete exposed buffers at every callback and on return. They cover zero
through seven tiles, both frame buffers, packet tag preservation, random
callback results and caller-register clobbers, and callbacks that modify current
descriptors, coordinates, count, serialized offsets and palette state. A hook
swaps the geometry-header pointer between its two volatile reads; X and Y must
use the respective headers. All callee-saved registers and SP are preserved.
Evidence: /tmp/pe-geom-load-mesh/{check.py,test.py,base.bin,target.bin,rtl/}.
Main debt records one pin, one barrier and two byte-relative additions.

Clean verification passes 341 tests and preserves main retail SHA-1
`452fb033f2eaa4b18aa20a5bca60b8125af3a37b`.
All 191 rebuilt overlays preserve their retail SHA-1 as well.
The audited report credits 2491796 semantic code bytes and 10738 functions
(70.18% of code). Total debt is 1292 pins, 1074 barriers and 155 NOPs;
ordinary ASM bodies and directives are unchanged.

### Item discard confirmation panel

`Menu_StepItemDetailPanel` (388 bytes at `0x80044F8C`) constructs the
mode-0x29 item-discard dialog and its action child using `MenuWidgetNode`.
It copies the selected item's label (or emits an empty 0xFF-terminated
string), appends table-4 message 4, and sizes/centers the widgets with a
minimum text width of 120. The second width query on the wide-text path is
preserved. The child initially selects action 1; the final callback is
`Menu_OnItemDiscardConfirm`. The active inventory list is selected twice,
as in retail, including after widget construction.

Stock native GCC 2.7.2 with `-G8 -fno-expensive-optimizations` and unmodified
MASPSX produces all 388 retail bytes. One local `$a0` pin and one empty
input barrier keep mode 0x29 materialized before copying the newly returned
parent pointer. Without that pin the candidate grows by four bytes; without
the barrier/pin pair the two setup instructions swap. Both constraints are
recorded in debt. There is no instruction ASM, NOP, EABI, or compiler change.
Shared declarations use existing named globals and the existing widget layout.

An independent MIPS/model harness passes 1000 cases for both the retail
function and compiled C. It checks null/non-null labels, the 119/120 width
boundary and an independently varying second width result, callback order
and arguments, active-list mutation during widget construction, all widget
bytes (including untouched fields), the final confirmation callback, and
preservation of the stack and callee-saved registers while callees clobber
caller-saved registers.

Acceptance: `make -j8 verify-clean` passes all 341 tests and the retail
main SHA-1; all 191 overlay binaries also retain their retail SHA-1.
The final declaration correction was rebuilt and rechecked against retail.
The audited report credits 2492184 semantic bytes and 10739 functions
(70.19% of code). Total debt is 1293 pins, 1075 barriers and 155 NOPs.

### Glyph sprite packet construction

`Draw_AllocSprite` (436 bytes at `0x8005EB64`) builds a 20-byte variable-size
textured sprite and an eight-byte draw-mode allocation. The recovered
`RenderSpritePacket` names the tag, packed color/code, coordinates, UV,
CLUT, width and height fields. `DrawGlyphDescriptor` has the eight-byte
stride established by the descriptor lookup; its final byte remains unnamed
reserved data. Both layouts have compile-time offset/size checks.

The allocator uses a strict end-before-arena-limit comparison, chooses the
normal or alternate color, and links each packet through its low 24 tag bits
while preserving the high byte. Draw mode selects `(glyph->mode & 3) << 7`
plus page 7. The ordering-table pointer is reloaded after `SetDrawMode`.
The retail allocation-failure path calls the assertion stub but continues
with a zero packet pointer; the reconstruction preserves that target-specific
behavior rather than adding a new early return.

Stock native GCC 2.7.2 with `-G8` and unmodified MASPSX matches all 436 bytes.
Four register pins and two empty compiler barriers preserve descriptor save,
tag construction and call-argument scheduling. Two provisional pins and one
barrier were removed with byte-identical output. There is no instruction ASM,
NOP, compiler modification or EABI. The constraints are counted in debt.

An independent MIPS/model harness passes 1000 cases for retail and C,
covering exact allocation boundaries (including equality), both color paths,
all descriptor bytes, full packet/arena/low-memory snapshots at callbacks and
return, preserved tag high bytes, assertion callbacks that replace the packet
cursor and glyph mode, and draw-mode callbacks that replace the ordering-table
pointer. It checks call arguments, stack and callee-saved registers while
callbacks clobber caller-saved registers. The draw-mode callback is a controlled
stub; these tests verify this caller's behavior, not the callee's implementation.
The two pointer-to-integer casts encode the GPU's 24-bit packet links and are
also recorded in debt.

Acceptance: `make -j8 verify-clean` passes all 341 tests and the retail main
SHA-1. All 191 rebuilt overlays retain their retail SHA-1. The audited report
credits 2492620 semantic bytes and 10740 functions (70.20% of code).
Total debt is 1297 pins, 1077 barriers and 155 NOPs; instruction ASM counts
remain unchanged.

### Digit quad packet construction

`Draw_EmitDigitSprite` (456 bytes at `0x8005F874`) allocates a 40-byte textured
quad, sets its four screen corners to a 5-by-7 rectangle, and selects the last
decimal digit for nonnegative inputs. Negative inputs use fixed UV (88,164).
Coordinates and UV additions wrap at the packet's 16-bit and 8-bit fields.
It selects the primary/alternate draw color, stores CLUT and texture page 7,
and links the packet into the ordering table while preserving tag high bytes.
Allocation uses the strict arena-end comparison; as in retail, a returning
assertion stub leaves the code continuing with a zero packet pointer.

The shared `RenderTexturedQuad` records the complete 40-byte GPU packet with
layout assertions. Stock native GCC 2.7.2 with `-G8` and unmodified MASPSX
produces all retail bytes. Only one `$a0` tag-value pin remains, counted in
debt, plus the GPU link pointer-to-integer cast. There are no empty barriers,
NOPs or instruction ASM. Four provisional pins and both barriers were removed
without changing the bytes. Three narrow volatile reads preserve y0/u0/v0
reloads; these are matching constraints, not evidence that the original source
declared the packet volatile. Three other provisional volatile qualifiers
were removed. Removing the final pin changes 21 instruction words.

An independent MIPS/model harness passes 2000 cases for retail and C,
including INT_MIN/INT_MAX, negative/single/multiple-digit inputs, all coordinate
and UV wraparound widths, both colors, strict allocation-limit boundaries,
and failure callbacks that modify cursor, color selection, screen position,
font origin and ordering-table pointer. It compares the whole packet arena,
low-memory failure destination and globals at callbacks and return, checks
untouched padding, and verifies the stack and callee-saved registers despite
caller-saved register clobbers.

Acceptance: `make -j8 verify-clean` passes all 341 tests and the retail main
SHA-1. All 191 rebuilt overlays preserve their retail SHA-1. The audited report
credits 2493076 semantic bytes and 10741 functions (70.22% of code).
Total debt is 1298 pins, 1077 barriers and 155 NOPs.
