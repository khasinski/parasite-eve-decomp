# func_80192F9C: initial recovered candidate

Not integrated. Linked asm-differ weighted Levenshtein score: **4909**.
Measured on darwine using stock GCC 2.7.2 (`-O2 -G0 -funsigned-char -mips1
-mcpu=3000`), stock MASPSX 2.56 (`--expand-div`) and the repository's scoring configuration.
Eight register pins and eight empty barriers are recorded below. There is no CPU ASM.
GTE transfers use
individual `gte_ctc2_0` through `gte_ctc2_7` macros.

The original target is the full `0x80192F9C..0x8019549C` function, restored
to scene_e19_2 by the extraction fix. The target has 2368 instructions; the
retained candidate has 2368. These counts are descriptive, not the match metric.

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

- Four pins: shared GTE transfer temporaries in t4/t5/t6 and the matrix
  pointer in t1. Phase and effect pointer are unpinned. No padding reserves.
- One memory barrier keeps position stores before the first matrix transfer
  block. One tied-operand barrier keeps the ring radius in a register.
  Two more tied-operand barriers preserve render-parameter and page-selector
  base pointers in draw state 2. Four slot-address barriers preserve the
  separate address/load sequence before GTE transfers. This totals eight.
  The spread/divisor barriers remain removed.
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

## Ring scalar reuse and pin removal

The ring now reuses radialScale, verticalScale and intensity, whose earlier
values are dead before these assignments and whose later values are assigned
after the loop. Its brightness argument retains explicit signed-halfword
narrowing, matching the retail extension sequence. Two unsigned sign-bit
rounding expressions elsewhere are written as signed division by two; this
rewrite alone preserves score 19144.

Reusing brightness alone scored 18429; reusing both scales as well scored
18399. Removing the matrix-pointer t1 pin lowers the combined result to
**18389**, independently recompiled and scored on darwine. Current candidate
debt is three pins (t4/t5/t6), four empty barriers and one GPU conversion
helper. No CPU instruction assembly was added. Removing each transfer-word
pin from the 19144 base worsened its score (20549, 19259, 19224), so those
remain. Omitting the ring brightness narrowing scored 18819 and was rejected.

## Recheck constraints after scalar reuse

A 120-second, 24-worker run in `permuter_ring/` on darwine completed 4585
iterations (96 rejected compilations). The exact-metric adapter reproduced
base score 18389 before searching; the best generated result scored 17998.
Several generated improvements moved statements across control-flow branches
or used uninitialized inputs and were rejected. Generated source was not
copied into the retained candidate. The timed run has stopped.

The useful hints were tested separately against the original source. Removing
the two spread barriers scored 17944 (first alone 18269, second alone 18064).
The remainder expressions remain signed C with divisor 512; these constraints
no longer improve the whole-function score. Removing an overwritten assignment
to sp48.x before the two endpoint draws lowers the retained score to **17618**.
Its value was replaced before any read or intervening call. Removing the
corresponding y assignment instead worsened that result to 17976.

Current debt: three transfer-word pins, two empty barriers and one inline GPU
conversion helper. From the 17944 intermediate, removing the memory barrier
scored 19089 and removing the radius barrier scored 18204; both are retained.
Named particle-prefix fields and resource/timer temporary reuse were also
tried; they left score 18389 unchanged and remain research-only.

## Palette ordering and arithmetic width

The aligned retail code at 0x80193DBC calls GetTPage before loading the palette
selector and row (0x80193DC8 and 0x80193DE0). Draw state 2 still had the row
read before that call. Correcting this remaining sequence reduces score
17618 to 16738; sharing the paletteRow variable preserves that score.

The retail row adjustment at 0x80193E08 adds four in a full register and passes
it directly to GetClut at 0x80193E10. A u16 local incorrectly truncates that
addition before the call. Using s32 for paletteRow, as required by this
sequence and the other repeated palette selections, reduces score to 14543.
This is a recovered arithmetic-width correction, not an extra constraint.

The remaining radial scale temporaries in draw states 1 and 2 now reuse
radialScale where their lifetimes do not overlap. Together they reduce score
to **14198**. Two sign-bit rounding expressions on this signed variable are
now ordinary division by two, preserving the same score. No pins or barriers
were added: debt remains three pins, two barriers and the GPU helper.
Changing all parameter aliases to struct fields in draw state 2 was tested
from the 16738 intermediate, scored 17059 and was not retained.

## Separate render tails and color branches

The m2c candidate sent draw state 4 backwards to a shared state-3 tail via
block_158. Retail has separate matrix submission sequences for these states
(the final one is at 0x801953A4..0x801953E8). Duplicating the C tail scored
12318 from 14198; directly referencing each branch's matrix and scale vector
scored 12278 and removed three temporary pointers.

The state-3 color setup now has ordinary if/else-if calls instead of shared
argument temporaries and goto block_84. This reproduces the separate argument
setup at 0x80194168 and 0x80194180 and lowers score to 12094.

Retail holds phase in s6 (for example 0x80194164 and 0x801941A0). A phase pin
to s6 lowers the combined score to **11630**. A plain register hint or
declaration reorder did not improve the unpinned intermediate. Separating
the ring height from verticalScale scored 12124 versus 12094 without the pin.
The retained source has four pins, two barriers and the GPU helper. This
additional phase constraint is candidate debt, to be retested as source
recovery proceeds. No CPU instruction assembly was added.

## Render-parameter scheduling pass

Using the existing RenderEffectParameters fields consistently within draw
state 2 scores 11605 from 11630. Reordering the two independent initial reset
writes at function end scores 11555. A memory barrier between the duplicate
extent writes made the result worse and was not retained. Retesting the
matrix-pointer pin and a combined texture-page array also worsened score.

A bounded 120-second, 24-worker run in `permuter_tail/` on darwine reproduced
base 11555, completed 4586 iterations (111 rejected compilations), and stopped.
The best generated score, 10955, added a repeated empty global condition and
was rejected. Another low-scoring variant used a pointer initialized only in
a different switch case and was rejected as invalid.

The valid retained hint reorders the palette and extent-y stores in draw
state 1, with no intervening call or dependent read. Applied manually to the
source and recompiled, this scores **11255**. No generated empty condition or
pointer was copied. Debt remains four pins, two empty barriers and one inline
GPU conversion helper; the candidate remains unintegrated.

## GPU helper site removal

Each of the 34 current gpuWord call sites was independently replaced by an
ordinary u16 cast and compiled/scored on darwine. Twenty-three replacements
were neutral or improving and were then tested together. The combined
retained result is **10655**, down from 11255. All conversions still preserve
the same low sixteen bits.

Three half-intensity submissions account for the improvement: the model at
D_8019B688 in draw state 3 and D_8019B68C in states 3 and 4. Each replacement
alone lowers score by 200; their combined effect is 600. The other twenty
removed helper calls preserve that result. Eleven helper calls remain.
The expanded original count was 34 after separating the two render tails.

Candidate constraints remain four pins and two empty barriers. Ring index,
radius and angle pins were tried individually from 10655, scoring 10835,
10640 and 11060. None is retained: the radius-only gain of 15 is left as a
research alternative rather than adding another constraint at this stage.

## Early timer reads and phase-pin removal

Retail reads the timer at 0x8019388C and 0x80193C78 before the corresponding
render-parameter stores. The candidate instead read it after those stores.
Moving the phase calculation to the start of draw states 1 and 2 corrects
this ordering and lowers score from 10655 to 9895 (10040 and 10510 when each
is changed independently). Removing the phase pin after that correction
further lowers the retained result to **9820**. A separate timer temporary
with the pin present does not improve 9895 and was not retained.

Nine signed rounding sequences on products are now ordinary division by 16.
Each replacement and all nine together preserved 10655 before the timer
change. This retains rounding toward zero for negative inputs and removes
reconstructed branches without introducing a helper or constraint.

Current candidate debt is three GTE transfer-word pins, two empty barriers
and eleven uses of the GPU conversion helper. From the unpinned 9820 base,
using render struct fields throughout state 1 scored 9940; using them across
the whole function scored 16744. Those wider replacements are not retained.

## Render base lifetimes

Retail retains the parameter base in s1 (0x80193C60), the page-selector base
in s2 (0x80193C70), and accesses the earlier selector at -16 from that base
(0x80193D98 and 0x80193DC4). The candidate now models these as ordinary
pointers with two empty tied-operand barriers to preserve their base
addresses across calls. Neither pointer is pinned to a register.

The selector view spans nine halfwords from D_800E11EA through D_800E11FA;
the local pointer starts at element 8, and accesses elements 0 and -8
relative to that pointer. Both accesses stay within this known address span.
The seven intermediate elements are not interpreted. The parameter pointer
uses the existing RenderEffectParameters layout; the initial scalar alias
writes retain the addressing form found in retail.

An ordinary palette-table pointer is established before the scale branch.
A larger experiment with additional scale and palette-pointer barriers
scored 9065, but removal checks showed both were unnecessary. Removing the
palette barrier improved the result to **8775**; removing the scale barrier
as well preserves 8775. Only the two base-pointer barriers are retained.
Current debt is three pins, four empty barriers, and eleven GPU helper calls.
No new CPU instruction assembly or register pin was added.

## Transition tail placement and GTE slot address

Copying the ring position in x/y/z order reduces score 8775 to 8759.
The shared state/timer update block was then moved from update state 0 to
state 3, matching its retail position at 0x80193520. Earlier transition
branches jump forward to it as in retail; score improves to 8508. The
assignments, branch conditions and return values are unchanged.

All four retail GTE setup blocks materialize the address of D_800BCFA4 before
loading its matrix pointer: 0x801936D0, 0x80193B70, 0x80193EC4, 0x8019475C.
An ordinary slot pointer with an empty tied-operand barrier at each site
reproduces this address/load separation and scores 7958. Pinning the loaded
matrix pointer to retail's t1 lowers the retained result to **7918**.
A slot-pointer pin or a plain inline identity helper without these barriers
scored 8518. Removing the existing initial memory barrier scored 8313; it
remains. New debt is four slot-address barriers and the t1 matrix pin.

Current total: four pins, eight empty barriers and eleven GPU helper uses.
All actual coprocessor instructions remain in individual GTE macros; no CPU
instruction assembly was added. Combined ring-index/radius/angle pin trials
all worsened the earlier 8775 baseline and were rejected.

## Reset extent fields

The final reset writes extent-x=16, extent-y=16, then extent-x=128 and
extent-y=16. Separate scalar aliases allowed GCC to interleave these stores
and the texture lookup differently. Using the existing RenderEffectParameters
extent_x/extent_y fields for these four assignments lowers score 7918 to
**7458**. The aligned reset sequence at 0x801953EC..0x80195464 now agrees
with retail; the function-wide frame size remains a separate mismatch.

Read-only, read/write and full-memory barrier probes each scored 8018, so no
reset barrier was retained. Replacing all reset aliases with struct fields
scored 8393 and was rejected. Individual palette/mode/page/extent substitutions
in other draw states were neutral or worse.

A phase-pin retest scored 7378 but remains research-only to avoid adding a
constraint for that small gain while source recovery continues. Separating
ring height scored 7488; pinning matrix vertical scale to s2 in that variant
scored 7723, or 7653 with the phase pin as well. Neither is retained.
Current debt is unchanged: four pins, eight empty barriers and eleven GPU
helper calls.

## Saved height conversion boundary

A bounded 120-second, 24-worker darwine run in `permuter_reset/` reproduced
base 7458 and completed 4836 iterations (112 rejected compilations). It has
stopped. The best generated score, 7148, moved a render-state write past
calls and was rejected. The generated 7188 variant combined an empty global
condition with a signed-short inline conversion; it was not copied wholesale.

The isolated signed-short identity helper at the saved sp30.y read scores
7123 in draw state 3 and **6778** when applied in both states 3 and 4. The
helper accepts and returns s16; its value is unchanged. No empty condition
is retained. This adds one fully inlined conversion helper at two sites to
candidate debt. The original four pins and eight barriers remain unchanged.

Alternatives: changing both saved-height locals to s32 or u32 scores 7108;
sharing one s32 savedY scores 7068; sharing the s16 helper result scores 7268.
These alternatives remain in scratch. Independent removal of every existing
barrier and pin worsened the 7458 base, as did replacing the GPU helper with
plain casts or changing texturePage to u16.

## Matrix-slot and phase registers; independent parameter stores

Fresh retained compilation scores **6453**, down from 6778. Pinning phase
in s6 and the GTE matrix slot address in v0 each independently scores 6698;
together they score 6618. Both pins are retained. Moving the existing pins
into the four GTE blocks was neutral (6778), as were the tested struct-field
substitutions in draw state 0. Removing the matrix pin scored 6818.

Three bounded darwine source-order probes tested 27 variants each, restricted
to seven independent parameter stores in draw states 0 and 1. The selected
orders score 6473 and then 6453. The stores address distinct halfwords; their
right-hand sides do not read those destinations. No operation crossed a call,
branch, or the following global-condition read. No unrelated generated source
changes were retained. Probe artifacts are order0_*, order1_* and order2_*
in the existing remote research directory; all probes have finished.

Current candidate debt: six pins, eight empty barriers, eleven gpuWord uses
and two savedHeight uses. The two helpers are fully inlined; nm shows only
func_80192F9C as executable function. The fresh alignment also confirms that
the frame is now 0x330, matching retail (the earlier 0x320 note is historical).
There is still no CPU instruction ASM and this is not a score-zero match.

## Draw-state 2 retained pointers

Pinning renderParams to s1 scores 6283; pinning pageSelector to s2 scores
6023. Combining the two scores **5828**, reproduced by a fresh retained
compile and linked weighted Levenshtein comparison on darwine. The variables
are used only in draw state 2. Their values, loads and stores are unchanged.
The target keeps these two bases in s1/s2 across drawing calls.

The existing tied-input barriers are still necessary: removing the selector
barrier scores 6888, removing the parameter barrier scores 6743, and removing
both scores 7803. Swapping pointer initializations scores 6448 without the
new pins and 6133 with them, so the existing initialization order is retained.
A barrier before the render-state read scores 6483 and was rejected; an
inline conversion there is neutral. Removing the matrix pin scores 6493
against the preceding 6453 base and was also rejected.

Current candidate debt is eight pins and eight empty barriers, plus the same
two fully inlined conversion helpers. No CPU instruction ASM was added.
Target and candidate each contain 2368 instructions; the score is still
nonzero and production retains the original assembly.

## Late rotation-X initialization

A bounded 120-second, 24-worker darwine run in permuter_ptr/ reproduced
base 5828, completed 4755 iterations with 95 rejected compilations, and
stopped. Its best generated candidate scored 5702. That candidate included
an unnecessary assignment to a const temporary around a GTE load; this was
not retained. Isolating only the later sp48.x = 0 assignment reproduced 5702.

Applying the same source-order change individually to analogous draw blocks
scored 5762, 5516, 5591, 5576, 5576, 5591, 5576 and 5576 against that base.
Combining the seven beneficial moves with the original isolated move scores
**4930**, confirmed by a fresh retained compilation. Each move crosses only
independent assignments to other rotation fields and rendering globals. None
crosses a function call, condition or read of sp48.x. All eight assignments
still precede the next texture-page query and matrix construction.

Debt remains eight register pins, eight empty barriers and the same two
fully inlined conversion helpers. No new inline assembly or helper is added.
The target and candidate each have 2368 instructions; no score-zero claim.
Additional palette-v1, intensity-s3 and radial-scale-s4 pins scored 6558,
11833 and 7398 respectively against base 5828 and were rejected.

## Rotation follow-up

A 120-second, 24-worker run in permuter_rotation/ reproduced base 4930,
completed 5106 iterations with 123 rejected compilations, and stopped.
Its best score 4805 removed a required global write from one draw state and
inserted a duplicate into another; it is invalid and was rejected.

The 4909 output combined an empty condition with moving sp48.x = 0 just
before the tpage store in the first saved-height draw block. Isolating only
that independent store reorder reproduces **4909**, now retained. No empty
condition is retained. A similar move in the second saved-height block also
scores 4909 by itself, but combining the two worsens to 5028. Other analogous
moves score 5064 or 5124. Debt is unchanged.

Pins for modelPhase, verticalScale, or both scored 4980, 5175 and 6656
against base 4930. Four chained-zero-assignment alternatives scored 5828,
5892, 5828 and 8768. None was retained.
