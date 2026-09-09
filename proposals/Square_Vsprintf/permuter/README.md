# Stock GCC 2.8.1 permutation search

The two inputs use the same formatter candidate. `unsplit/compile.sh` passes
`-mno-split-addresses`; `split/compile.sh` allows GCC's default address splitting.
Compiler flags live in the wrapper because the permuter strips C comments.
The `args` preprocessor alias is expanded in both inputs for the permuter parser.

On 2026-09-09 these inputs were launched on `darwine` in
`/home/hasik/sprintf-permuter-20260909`, with 28 local workers. The initial
unsplit object was copied back and independently checked with objdiff:
99.16058%, reproducing the local candidate. Permuter baseline penalties are
160 (unsplit) and 365 (split); these are not percentages.

The directory also needs:

- `tools/cc1`: unmodified Linux `gcc-2.8.1-psx` from decompals/old-gcc release
  0.17, as selected by the repository's `setup_stock_cc281.sh`;
- `tools/maspsx`: a copy of this repository's pinned MASPSX checkout;
- `tools/permuter`: a copy of this repository's decomp-permuter checkout;
- `venv/bin/python` with the permuter dependencies;
- system `mipsel-linux-gnu-as` and `mipsel-linux-gnu-objdump`;
- `target.o` in each input directory, copied from
  `expected/build/USA/src/main/psyq/libc/Square_Vsprintf.c.o`.

Run from that directory:

```sh
venv/bin/python tools/permuter/permuter.py unsplit split --debug
venv/bin/python -u tools/permuter/permuter.py unsplit split -j 28 \
  --better-only --stop-on-zero --stack-diffs > run.log 2>&1
```

Use `--debug` for baseline compilation: the permuter removes comments and
canonicalizes C before invoking these cc1 wrappers. The wrappers intentionally
consume that preprocessed input rather than arbitrary raw C.

Inspect saved `output-*` candidates, then compile promising results through
local `tools/scripts/cc.sh` with the corresponding GCC and address-splitting
markers restored. Validate the complete function, relocations and jump table
with objdiff, inspect semantic changes, and require the full retail SHA checks
before promotion. A permuter score of zero alone is not sufficient evidence.

The live search and its outputs are experimental; production remains ASM.

The initial run used `--best-only` and produced two penalty-120 candidates.
Both were rejected: one moved the hexadecimal prefix's `--src` outside the
alternate-form condition; the other did the same for the decimal sign prefix.
Both change the source pointer passed to `memmove` when no prefix is present.
The search was restarted without `--best-only` so an invalid low penalty
cannot hide other improvements over the baseline. All unique improvements
are now saved, still requiring semantic review.

## Corrected unsplit seed

`darwine:/home/hasik/sprintf-permuter-20260909/unsplit-refined` uses the
`constrained_gcc281.c` source from commit 2c42e1d6 with comments removed and the
`args` preprocessor alias expanded to `argState[0]`. It retains the unsplit
compiler wrapper, target and settings. Its Linux-built baseline was copied
back and independently scored at **99.500916%** with local objdiff. The
permuter's debug baseline penalty is **270**; this metric must not be confused
with objdiff percentages or used to compare semantic validity.

The new search runs with two workers at nice 10, `--better-only --stop-on-zero
--stack-diffs`, logging to `run-unsplit-refined.log`. PID 447715 was verified
live and advancing past 179 iterations at launch. Existing unsplit/split and
split-refined searches were left running. Use current process state when
checking these jobs; recorded PIDs alone do not prove liveness.

Local follow-up tried a digit-table a3 pin and eight combinations of tied
source-pointer barriers around the hexadecimal prefix stores. None improved
99.500916%; no such constraints were retained. The remaining diff includes
uppercase table-address scheduling and prefix pointer/store instructions,
plus jump-table symbol differences that require linked-byte validation.

## Refined run audit, 2026-09-10

The existing unsplit-refined process was confirmed live at approximately
92,899 iterations. Three newly saved outputs were compiled locally with the
same stock flags and tested against retail. Output 255-1 fails hexadecimal
zero formatting; 255-2 fails decimal zero formatting; 235-1 raises an unaligned
read emulator error on unsigned zero formatting. None is accepted. Source
SHA-256 hashes, independent objdiff scores and outcomes are recorded in
unsplit-refined-audit.json. The maintained 99.500916% candidate is unchanged.


## Prefix-corrected split search (2026-09-10)

A separate input at `darwine:/home/hasik/sprintf-permuter-20260909/split-prefix-20260910`
uses `constrained_split_gcc281.c` from f716c7cd. Its source SHA-256 is
`ad487abefd5b039662f40abdb960c3c923ce8b3369689d271befebbdd2138596`.
Block comments and the `#define args argState[0]` line are removed, then each
remaining `args` identifier is expanded to `argState[0]` (the final undef is
kept as `#undef args`). Expanded input SHA-256:
`d4a14704c98f8b4c04ee3f26574c9f434fbedbd78d1713eb0f7fd375c4eb22ea`.
The compile wrapper, target and settings are copied from `split-refined`.

Baseline permuter penalty is 325. Independently compiled on darwine, the input
scores 99.39449% in local objdiff and passes all 1,471 oracle cases after the
oracle's cross-host alignment fix. Its linked text and rodata equal the local
split build. The baseline penalty is not a byte-match percentage.

The first short launch was explicitly stopped while diagnosing oracle
placement. After validation, a new bounded run was confirmed live with timeout
PID 693700 and permuter PID 693701, progressing past 372 iterations. It uses:

```sh
timeout 30m nice -n 15 venv/bin/python -u tools/permuter/permuter.py \
  split-prefix-20260910 -j 2 --better-only --stop-on-zero --stack-diffs
```

Output is in `run-split-prefix-20260910-validated.log`. This limits the trial
to 30 minutes and two low-priority workers without stopping the older searches.
PIDs and elapsed times are observations, not proof the process remains live;
recheck the process and saved outputs before continuing. No output from this
trial is accepted without local compilation and behavioral verification.
