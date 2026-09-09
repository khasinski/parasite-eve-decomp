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
