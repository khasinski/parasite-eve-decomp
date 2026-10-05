# FieldEng_TexturedRibbon candidate

`func_800D3114`, retail text at 0x800D3114, 2536 bytes. This candidate is WIP,
not a match and not part of the production build.

All six rotation/translation load pairs and the depth arithmetic have been
expressed in C; the GTE transfers and commands use individual instruction macros.
The code was compiled only on darwine with stock native GCC 2.7.2 and stock
MASPSX 2.56 (`--expand-div`).

| Version | Score | Bytes | Meaning |
| --- | ---: | ---: | --- |
| Production baseline | 0 | 2536 | Existing legacy macros, retail-identical |
| Full initial C expansion | 8956 | 2552 | Individual GTE instructions throughout |
| Shared local matrix pointer and repeated vertex addresses | 7542 | 2532 | `lifetime.c` |
| Retained candidate | 6103 | 2556 | `pin3.c`: local matrix, loop count and right-vector pointer pins |

The retained version has 22 pins and 12 empty constraints. These are prospective
matching debt only, not added to the production baseline. They have not been
minimized because the candidate does not match.

Main differences: allocation and lifetime of matrix translation, unused screen
output and left-vector pointers; loop-invariant addresses are kept in saved
registers instead of the retail spill slots. The count pin also moves its sign
extension into s7 too early in the prologue. Pinning the packet pointer worsens
the result. A direct camera-pointer pin to t7 shifts other reload registers and
is not retained. Merely changing empty barriers to non-volatile is insufficient.

Diagnostic partial conversions (`matrices.c`, `local_matrices.c`, `local_bound.c`)
still contain legacy macros and are not complete C candidates; their scores
must not be reported as the score of a full conversion.

Research artifacts: `scratch/textured_ribbon/` locally and
`/home/hasik/fx-search-archives/textured_ribbon/` on darwine. `retained.c` matches
this checked-in source. Linked score JSON/diffs and retail byte comparisons are
available for each trial. `pins.py` records the 16 lifetime-pin combinations;
`pure0..7` record the barrier variants. No permuter was started for this trial.
