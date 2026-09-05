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
headers/macros. COP2 expressions still require manual review for surgical scope.
The retrospective also found ordinary CPU packing, shifts and stores in seven
GTE-named helpers. Their callers, including direct `.inc` template users, are
explicitly quarantined by the classifier. This list is a stopgap for known
violations, not a complete header-expansion audit. LoadAverageByte and
LoadAverageCol now use only MAC transfers in the replacement helpers; their
shifts and byte stores are C. The remaining 29 affected functions lose semantic
credit until corrected. The two repaired functions were previously counted,
so their repair is not two newly added functions.

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
