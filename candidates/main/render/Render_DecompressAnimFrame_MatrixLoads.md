# Render_DecompressAnimFrame: initial rotation load

Full C candidate for the remaining CPU matrix transfers, using individual
GTE wrappers. Not promoted: linked asm-differ score **30**, 2292/2292 bytes,
with six register differences. The production version keeps the first legacy
rotation-load macro and matches all retail bytes.

- Entry: `0x8007041C`; retail file range: `0x60C1C:0x61510`.
- Stock native GCC 2.7.2, existing `-G8`; stock MASPSX 2.56.
- Retail loads the matrix pointer into `t2` at `0x800704B8`; the candidate
  uses `v0`. The five loads at `0x800704C0`, `C4`, `D0`, `D4`, `D8` consequently
  use `v0` as their base. All other instructions match.
- GCC's local-allocation dump explains the difference: the legacy macro's
  single pointer operand is folded to a memory expression, then materialized
  in `t2` by reload. Five C accesses retain a pointer pseudo allocated to `v0`.
- Pinning that pointer to `t2` fixes the initial sequence but changes register
  allocation in the later face-rendering loop (score 225). Broad low-register
  clobbers also disturb the later allocator. Additional pins for the affected
  loop variables make the result worse.
- Inline helpers, volatile loads, split pointer lifetimes, and `register`
  hints did not eliminate the six differences. The partial production version
  was minimized separately; the complete candidate still has score 30 after
  applying those removals.

Research: `scratch/anim_matrix` locally and
`/home/hasik/fx-search-archives/anim_matrix` on darwine. `evaluate.py` links
absolute symbols and checks both asm-differ and retail byte equality.

A bounded 90-second permuter run on darwine completed 9237 iterations
(925 rejected/failed compilations) without improving score 30. It used
`--stack-diffs --no-ignore-branch-targets`, 24 workers, and the linked retail
target. No permuter is intended to remain running for this candidate.
