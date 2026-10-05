# FieldEng_HistoryTrail candidate

`func_800D1384`, retail main text 0x800D1384–0x800D1ADF, 1884 bytes.
This is a WIP candidate, not production and not a byte match.

Stock native GCC 2.7.2 and stock MASPSX 2.56 (`--expand-div`) on darwine:
- Original production source: score 0, identical to retail.
- This candidate: score 10, 1884 bytes, one differing instruction.
- At 0x800D1724 retail uses `sw t4,0(t8)`; candidate uses `sw t4,0x20(sp)`.

All CPU matrix loads and depth arithmetic are C. Each GTE transfer/command
has its own macro. The candidate has ten register pins and one empty
memory barrier; these are prospective debt, not added to the production baseline.

A shared `depthOut` pointer within the valid-history branch removes a second
pointer reload, but GCC folds the final C store into stack-relative addressing.
Making that pointer opaque inside the loop prevents folding but removes the
retail loop-invariant spill and shrinks/rearranges the frame. Hoisting an opaque
pointer before the loop gets the store right but moves initialization before the
zero-count branch. An explicit guard plus a do/while loop reproduces the body,
but shrinks the frame from 128 to 120 bytes. No padding or CPU inline ASM was
introduced to conceal this difference. Pinning count to s7 fixes the count/history
register swap; pinning the view to t7 preserves the retail reload-register choices.

Research artifacts: `scratch/history_trail/` locally and
`/home/hasik/fx-search-archives/history_trail/` on darwine. `finish1.c` is this
candidate (without this header); its linked JSON/diff records the score and byte
comparison. `outer.c`, `guarded.c`, `do_loop.c` and the `opaque*`/`flow_*` trials
record the pointer and loop experiments. No permuter was started for this trial.
