# FieldEng_HistoryTrail — matched and promoted

`func_800D1384` is now in `src/main/engine/FieldEng_HistoryTrail.c`.
Stock native GCC 2.7.2 and stock MASPSX 2.56 (`--expand-div`) on darwine
produce score 0 and exact retail bytes for all 1884 bytes.

The former score-10 candidate differed only at 0x800D1724: GCC folded the
indirect depth store into a stack-relative store. An empty, non-volatile tied
pointer constraint at the start of the `for` loop prevents that fold while
allowing loop-invariant motion. Putting the constraint inside the conditional
branch or marking it volatile prevents the required hoist. The original `for`
loop is retained; no padding, compiler changes, or CPU instruction ASM is used.

After 78 single/pair removal trials, eight register pins and one empty constraint
remain, tracked in production debt. The stale candidate source has been removed.
Research artifacts remain in `scratch/history_trail/` locally and
`/home/hasik/fx-search-archives/history_trail/` on darwine (`loopopaque0.c`,
`minimal.c`, linked score/byte comparisons and `main_verify.log`).
