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
