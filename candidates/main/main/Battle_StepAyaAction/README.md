# Battle_StepAyaAction — nonmatching candidate

Retail function: `0x80024A3C`, file offsets `0x1523C..0x15CBC`, 2688 bytes.
The manifest still selects the original ASM. This candidate is not a completed decompilation.

The candidate reconstructs the 17-state action using `BattleEntity`, `BattleInitSlot`,
`EnemyCombatant`, and the shared render-object types. It replaces the previous
helper-based candidate, whose state mapping and range-field interpretation differed
from the assembly. The range calculation needs signed division by ten.

Verified on darwine with stock native GCC 2.7.2 and stock MASPSX (compatibility
profile 2.77), using the flags in the source. No compiler or assembler modifications.
Only register declarations and empty compiler barriers use `asm`; no CPU instructions.

Current checkpoint: **320 linked asm-differ score**, **320 permuter Levenshtein
score**, branch destinations included, stack differences included. These scoring
scales are different. Candidate size is 2688 bytes, but its bytes do **not** match.
The retail `.text` SHA-256 is `2a01fbf4b2de27be63246292791421b9ef373c8e801939d96e7130f296f8a990`.

Candidate debt: **31 register pins**, **9 empty barriers**. They are recorded
here because the production debt scanner only scans `src/` and `include/`.
The production debt baseline must be updated if this source is promoted.

Small state globals are tentative definitions so stock MASPSX receives COMMON
size metadata. Imported game globals retain absolute addressing. Before promotion,
verify both their final storage/symbol resolution and the entire retail executable.
The generated jump table at `0x80010824` must also match; text scoring alone is
insufficient.

Reproduction workspace on darwine: `/home/hasik/fx-search-archives/Battle_StepAyaAction`.
`compile.py combined_8_9` compiles the checkpoint; `evaluate.py combined_8_9`
links against retail addresses and checks text bytes. `permuter_r3/compile.sh`
is the stock-toolchain compile/link wrapper; its `base.c` was checked to compile
identically to the original seed before launch. Permutations run only on darwine.

Remaining differences are nine diff rows in states 9 and 15: constant scheduling,
ordering of the first target flag store, and the order of two immediate loads. Keep the best source and scores together; do not mark a same-size
candidate as matched.

Validation before this checkpoint: production `make -j32 verify` on darwine
passed the whole-main executable byte comparison. Source mapping, source policy,
crutch debt, and organization debt checks passed. Production still uses ASM here.
