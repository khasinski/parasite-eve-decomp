# func_80192F9C: initial recovered candidate

Not integrated. Linked asm-differ weighted Levenshtein score: **83843**.
Measured on darwine using stock GCC 2.7.2 (`-O2 -G0 -funsigned-char -mips1
-mcpu=3000`), stock MASPSX 2.56 and the repository's scoring configuration.
There are no register pins, empty barriers or CPU ASM. GTE transfers use
individual `gte_ctc2_0` through `gte_ctc2_7` macros.

The original target is the full `0x80192F9C..0x8019549C` function, restored
to scene_e19_2 by the extraction fix. The target has 2368 instructions; this
first candidate has 2384. These counts are descriptive, not the match metric.

Recovered layout evidence:

- The second argument contains three eight-byte vectors at 0, 8 and 16,
  followed by state and timer halfwords at 24 and 26.
- Channel offset 8 points to an object pointer; the object's first word is
  used as flags. Other channel/object fields are not modeled here.
- Stack vector and matrix fields were grouped using call prototypes and
  their load/store offsets. This prevents separately allocated scalar locals
  from being passed as if they were contiguous vectors.
- The first eight-byte initializer and two four-byte color initializers are
  packed copies. The initial m2c run used the wrong endian setting; recovery
  now uses `mipsel-gcc-c`.
- `func_80071A54` is `rand`, with no arguments. Supplying that prototype
  eliminates m2c's false reads from unset argument registers.

For analysis only, unsupported `ctc2` instructions were represented as stores
to distinct symbolic control registers. Their recovered assignments were
then replaced by the corresponding individual GTE macros. These analysis
substitutions are never used as the scoring target. The target object is
assembled directly from the original disc slice.

The source remains an initial m2c-derived candidate, not reviewed retail-style
C. Temporary names, casts, control flow, unknown global views and signedness
still need examination. Successful compilation is not a semantic-equivalence
claim. Keep original assembly in production until the candidate is validated
and reaches score zero.

Reproduction artifacts on darwine:
`/home/hasik/fx-search-archives/scene_e19_80192F9C/` contains the context,
analysis input, `compile.py`, `score.py`, target, linked candidate and score.
The same source preparation artifacts are under local
`scratch/scene_e19_80192F9C/`. The retained source/header here contain everything
needed for candidate compilation with the project's include paths.
