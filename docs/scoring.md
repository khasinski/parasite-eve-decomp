# Candidate scoring

Use the project's virtual environment and the pinned asm-differ dependency:

```sh
.venv/bin/pip install -r requirements.txt
.venv/bin/python tools/scripts/score_decompme.py target.o candidate.o
```

The script uses Levenshtein alignment and the default MIPS configuration from
[decomp.me's diff wrapper](https://github.com/decompme/decomp.me/blob/main/backend/coreapp/diff_wrapper.py).
The reported score includes asm-differ's weighted penalties; it is not an
unweighted edit distance between machine words. `--target-symbol` and
`--candidate-symbol` select a function when the objects contain multiple functions.
Use equivalent target and candidate objects, including their relocation and rodata
information. Different target symbols, diff flags, or asm-differ versions on a
remote scratch can affect its score.

Always pass `--algorithm levenshtein` to decomp-permuter. Its weighted score is
separate from this script's score; do not assume the numbers are interchangeable.
Any raw or relocation-masked word distance must be labelled as diagnostic data,
not a decomp.me score.

A score of zero is not sufficient to promote a function. Verify the complete
linked overlay against its retail SHA-1 before counting a byte match.

## Current matching goal

The immediate goal is score 0 for every real function using the Levenshtein
configuration above. Leave already matching source alone, including existing
CPU-ASM helpers; removing that debt and reorganizing types/TUs is deferred.
Build verification remains a separate integration gate, not a substitute score.
Run compilation and permutation searches on darwine with stock GCC and MASPSX.

[The matching inventory](match_inventory.json) records the 44 entries excluded
by the report for commit `94715a2c3` (green CI run `37306501681`). These are
**not 44 proven unmatched functions**. Fresh linked Levenshtein checks on darwine
show score 0 for all seven functions in the five `asm_constrained` units. They
remain excluded by the report's semantic-C policy and must not become cleanup
targets under this goal. Two additional entries are text-resident data, and
`render_clip.bin` duplicates a truncated slice of `fx_common.bin`. Other entries
still require classification; a missing score in the inventory means unknown,
not zero. Do not claim 100% from this partial audit.
