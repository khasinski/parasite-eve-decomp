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
