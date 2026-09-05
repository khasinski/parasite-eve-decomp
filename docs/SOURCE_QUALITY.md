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
code-byte metric and full linked SHA-1 remain stronger evidence. Known
text-resident data is forcibly represented as data on the target side.
After objdiff runs, `make report-audit` independently verifies that the
published matched-code and matched-function totals contain only `semantic_c`.

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
The other flagged families remain constrained until repaired. These are
eligibility corrections, not changes to retail code targets or evidence that
the matching binaries ceased to match.

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
