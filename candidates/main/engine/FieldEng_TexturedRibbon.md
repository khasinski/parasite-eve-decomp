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
| Previous candidate | 6103 | 2556 | `pin3.c`: local matrix, loop count and right-vector pointer pins |
| Retained candidate | 5273 | 2560 | `vplace1.c`: shared left/screen pointers, count allocation constraint, earlier final V store |

The retained version has 21 pins and 13 empty constraints. These are prospective
matching debt only, not added to the production baseline. They have not been
minimized because the candidate does not match.

Main differences: allocation and lifetime of matrix translation, unused screen
output and left-vector pointers; loop-invariant addresses are kept in saved
registers instead of the retail spill slots. The count pin previously moved its sign
extension into s7 too early in the prologue; it has been replaced by four
input references in an empty constraint, but allocation is still different. Pinning the packet pointer worsens
the result. A direct camera-pointer pin to t7 shifts other reload registers and
is not retained. Merely changing empty barriers to non-volatile is insufficient.

Diagnostic partial conversions (`matrices.c`, `local_matrices.c`, `local_bound.c`)
still contain legacy macros and are not complete C candidates; their scores
must not be reported as the score of a full conversion.

Research artifacts: `scratch/textured_ribbon/` locally and
`/home/hasik/fx-search-archives/textured_ribbon/` on darwine. `retained_v2.c` matches
this checked-in source. Linked score JSON/diffs and retail byte comparisons are
available for each trial. `pins.py` records the 16 lifetime-pin combinations;
`pure0..7` record the barrier variants. Two 90-second permuter runs were completed on darwine with 24 workers:
11,612 iterations (1,642 compile errors), then 11,832 (1,688 compile errors).
No workers remain. Internal permuter scores differ from the linked comparison
above; every retained change was independently recompiled and scored with the
same linked evaluator as the original candidate. The expanded-source outputs
require the `--expand-div` flag when recompiled.

The first result reduced to a shared `leftPtr`; an empty conditional generated
by the permuter was unnecessary and discarded. Manual alias trials added
`scratchScreen`, and count weighting removed its hard pin. The second run found
an earlier store of the final packet V coordinate. Moving that store outside
the matrix-load block, just before the MAC transfers, improved the linked score
to 5273. No raw expanded GTE ASM was copied into the retained macro-based source.
`permuter/` and `permuter_round2/` preserve raw results; `left_ptr.c`,
`countweight4.c`, `early_v.c`, and `vplace1.c` record the interpreted changes.
