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
