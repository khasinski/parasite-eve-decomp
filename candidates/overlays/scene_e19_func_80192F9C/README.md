# func_80192F9C: initial recovered candidate

Not integrated. Linked asm-differ weighted Levenshtein score: **19144**.
Measured on darwine using stock GCC 2.7.2 (`-O2 -G0 -funsigned-char -mips1
-mcpu=3000`), stock MASPSX 2.56 (`--expand-div`) and the repository's scoring configuration.
Four register pins and four empty barriers are recorded below. There is no CPU ASM.
GTE transfers use
individual `gte_ctc2_0` through `gte_ctc2_7` macros.

The original target is the full `0x80192F9C..0x8019549C` function, restored
to scene_e19_2 by the extraction fix. The target has 2368 instructions; the
retained candidate has 2367. These counts are descriptive, not the match metric.

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

## Control-flow pass

The first recovery placed draw (mode 2) before update (mode 1). Replacing the
outer reconstructed conditionals with an explicit mode switch ordered 0, 1, 2
reduced score from 83843 to 56674. Other case orders scored 83298 (0, 2, 1)
and 63459 (1, 0, 2); an ordered if/else chain scored 57804.

Ten negative-rounding sequences whose temporary is not subsequently read are
now expressed as signed division by 32. This retains score 56674. Simplifying
the other shift/rounding pattern increased score to 57714 and was not retained.
A single experimental effect-pointer pin to s5 scored 55522, but is kept only
in scratch: allocation work is premature while source recovery is incomplete.
The retained candidate remains unpinned.

## Arithmetic and source-shape pass

The original at `0x80193F9C..0x80193FC4` computes signed `(cosine * 2) / 3`.
The first m2c recovery mistakenly emitted that division and then subtracted
its sign again. For cosine -4096 that yielded -2729 instead of -2730.
The candidate now performs the correction only once. The ring loop is a
sixteen-iteration `for`, replacing reconstructed induction temporaries and
`do/while`. Together these changes score 55922.

The two particle-spawn paths use a divisor held in a register for `% 512`,
including signed division guards. Hiding only the divisor constant from GCC
with empty tied-operand barriers reproduces that instruction family and lowers
score to 49547. The source retains the existing shared `RoomSoundSlot` view
for the sound/archive pointer, lowering score further to 48132. This allows
GCC to retain the address across repeated resource calls, as retail does.

### Candidate debt

- Two empty tied-operand barriers, one before each particle allocation, keep
  the spread value in a register instead of constant-folding remainder by 512.
  They contain no CPU instruction and do not change the spread value.
- Four pins: shared GTE transfer temporaries in t4/t5/t6 and the matrix pointer
  in t1. The effect pointer is unpinned. No padding reserves.
- One additional memory barrier keeps position stores before the first matrix
  transfer block; together with the two divisor barriers this totals three.
- `--expand-div` is an existing stock maspsx option, not an assembler patch.

These are research-candidate costs, not production debt: the function is still
original assembly in the build. Record them in the production ratchet if and
when this candidate reaches zero and is integrated.

## Matrix-transfer pass

Grouping the C loads as two rotation words, three rotation words and three
translation words reduces score from 48132 to 45032 without pins. Pinning only
the three transfer temporaries to retail's t4/t5/t6 lowers it to 42447. One
shared set of declarations produces the same score as four separate sets.
Pinning the effect pointer to s5 gives the retained **40927** candidate.

Single-pin removal checks on that candidate score 43317 (without t4), 41017
(without t5), 41122 (without t6), and 42447 (without s5). A spread/divisor pin
to s1 does not improve the score and is omitted. These are local removal
checks, not proof that pins are intrinsically necessary in every source shape.

Unifying all render parameter globals into the shared aggregate was also tried;
that variant scored 53413 from the earlier 48132 baseline and is not retained.

## Texture selection pass

The page-index views at D_800E11EA and D_800E11FA are now explicitly typed as
halfword array elements. The previous scalar declaration prevented GCC from
retaining the index address across calls; the byte addresses and values are
unchanged. The alternate access at D_800E11FA - 0x10 is expressed directly as
D_800E11EA[0]. These changes reduce score from 40927 to 37023.

The ten repeated texture setup sequences now call GetTPage (func_80077A64),
combine the returned page bits with the page table, then read the palette row.
This follows the retail call/read ordering and reduces score to 30618. The
palette row is represented by one reused `paletteRow` variable, with no score
change. Reusing one page variable as well scored 30763 and was not retained.
No additional pins or barriers were added in this pass.

A stock GCC 2.8.1 probe of this source scored 84105; GCC 2.7.2 remains the
retained compiler (30618), recompiled and rescored after the probe.

## Recovered scalar roles

Nonoverlapping m2c temporaries now share named variables for radial scale,
vertical scale, intensity, phase, model phase and texture page. The two scales
reduce score from 30618 to 26948; phase and intensity reduce it to 25818;
sharing texture page reduces it to 25808. Sharing model phase keeps that score.
The signed-halfword intensity used by the ring remains separate.

These merges preserve each assignment and use: no old value is used after the
next merged assignment on any affected path. They eliminate temporary names
without adding constraints. Simplifying the remaining signed half-division
expressions was tested against the scale-only variant (26948), scored 27333,
and was not retained. Pins and barriers remain four and two respectively.

Retesting pins after scalar reuse scored 28198 without t4, 25898 without t5,
26003 without t6, and **25738 without the s5 effect pin**. The latter improvement
is retained: the effect pointer is once again an ordinary function argument.
Current candidate debt is three pins and two empty barriers.

## Transfer ordering refinement

The final translation-word load now follows `gte_ctc2_5`, matching the retail
instruction order. One shared matrix-pointer pin to t1 and one memory barrier
before the first transfer group reduce the score to 24163. Entry barriers in
the other three groups were removed (the larger variant scored 24188).
Four experimental barriers after `gte_ctc2_5` proved unnecessary: removing each
individually and then all four preserved 24163. They are not retained.
Current candidate debt is four pins and three empty barriers. CPU ASM remains
absent; actual GTE instructions are still individually wrapped.

## Bounded permuter pass and GPU narrowing

A 120-second, 24-worker run on darwine completed 4758 iterations (114 rejected
compilations), with a best generated score of 22933. The research adapter uses
`tools/scripts/score_decompme.py` for every candidate, not the permuter's own
score implementation. Debug mode first reproduced the retained base score
24163. GCC and maspsx were unchanged. The search stopped at its time limit;
no ongoing permuter was left behind.

Its useful change was a narrowing boundary for GPU arguments. Plain u16 casts
scored 24163, while applying a small static inline `gpuWord(u16)` helper at all
33 existing masks scored **20963**. The helper returns its argument unchanged;
conversion to u16 occurs at the parameter boundary. This preserves the low
sixteen bits, including for negative inputs. The generated arbitrary temporary
and external inline helper from the search were not copied into the candidate.

Additional candidate debt: one scalar conversion helper used at 33 sites.
It contains no assembly. `nm` confirms that the candidate object defines only
`func_80192F9C` as executable code; `gpuWord` is fully inlined. Existing debt
remains four pins and three empty barriers. A separate phase-pin trial on the
previous base scored 23709; that pin was not retained.

Search artifacts and its exact-scoring adapter are in the research directory's
`permuter/` subdirectory on darwine; local preparation scripts are in
`scratch/scene_e19_80192F9C/permuter/`.

## Ring radius pass

Retail loads radius 2000 into s1 at 0x80193F6C and uses register
multiplication for both sine and cosine in the sixteen-vertex ring. A plain
constant instead generates shifts and additions. One empty tied-operand
barrier on the named `ringRadius` preserves register multiplication and
reduces the linked weighted Levenshtein score from 20963 to **19144**.
No additional register pin is used. Current candidate debt is four pins,
four empty barriers and one fully inlined GPU conversion helper.
An alternative inline multiplication helper scored 19399 and was not retained.

A second bounded 120-second, 24-worker darwine permuter run started from
20963 and completed 4737 iterations with 85 rejected compilations. Its best
score was 20079, using an empty repeated-global condition; that artificial
condition was rejected. The run stopped at its time limit. The manually
recovered radius variant was independently rebuilt and rescored on darwine.
