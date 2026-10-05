# func_80192F9C: initial recovered candidate

Not integrated. Linked asm-differ weighted Levenshtein score: **787**.
Measured on darwine using stock GCC 2.7.2 (`-O2 -G0 -funsigned-char -mips1
-mcpu=3000`), stock MASPSX 2.56 (`--expand-div`) and the repository's scoring configuration.
Ten register pins and eleven empty barriers are recorded below. There is no CPU ASM.
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

## Constraint audit at 4909

Fresh individual pin removals, in declaration order (pageSelector,
renderParams, matrixSlot, matrix, w0, w1, w2, phase), scored 5364, 5104,
4989, 4949, 6499, 5029, 4989 and 4989. Every removal worsens the retained
4909 base. Individual empty-barrier removals, in source order, scored 5059,
5074, 5074, 5969, 5824, 5074, 5169 and 5239. No constraint was removed.

Widening both saved-height locals to s32 was neutral (4909), as was sharing
one s32 saved-height local. Sharing one s16 local scored 5399. Widening the
helper return scored 5239; removing it scored 5589. These source alternatives
are research-only. A C signed-64-bit multiply-high implementation of the two
remainder-by-800 expressions, with a t1 high-word pin, scored 5994 and added
six instructions; it was rejected. No CPU instruction ASM was introduced.

All trials ran on darwine using the existing stock toolchain and weighted
Levenshtein scorer. The canonical candidate was recompiled afterward and
reproduces 4909. Artifacts are unpin4909_*, drop4909_*, *_4909 and
remainder800_high in the existing research directory. No search is running.

## Scoped palette coordinate and bounded exhaustive searches

The retained candidate now scores **4889**. In draw state 2, the palette
coordinate used for the streak model is a block-local paletteY instead of
the function-wide paletteRow. The arithmetic, globals read and call order
are unchanged. A fresh compile of the final named/indented source reproduces
4889. No new pin, barrier or helper is added.

An exhaustive darwine search covered all 5040 orders of the seven independent
parameter stores in draw state 0. All compiled; none improved the 4909 base.
Results and best source snapshots are in store_order_full/. A separate
120-second, 24-worker permuter run restricted to the ring setup and loop
completed 14,075 iterations with nine rejected compilations, with no better
score. Its artifacts are in permuter_ring_focus/; the run has stopped.

Early selector loads and guards, tied ring index/angle pins, longer angle
lifetimes, model phase/height pins and guards, and swapping rotation-X with
the parameter06 zero store were neutral or worse. None was retained.
Per-state local-variable trials scored 4909 (modelPhase), 4949 (verticalScale),
5912 (radialScale), 6537 (intensity), 4889 (paletteRow), 4909 (texturePage),
and 6260 (all six). Isolating just the retained palette scope achieves the
same improvement with the smallest source change. Trial families are
early_page_*, ring_bound_*, ring_lifetime_*, model_bound_*, matrix_clobber_*,
ring_split_*, swap_zero_*, state_locals_* and palette_scopes_* in the research
directory. The production function remains original assembly.

## Vector copy and draw-expression pass (4491)

Restoring the ordinary component copy from `effect->origin` into `sp48`,
followed by the X/Y offsets, reduces 4889 to 4646. The generated sequence
now retains the initial X store present in retail. The old two halfword
temporaries are removed. All six component orders and both offset orders
were compiled on darwine; XYZ followed by X then Y was best.

A 120-second, 24-worker stock decomp-permuter search (`permuter_draw_4646/`)
completed 5163 iterations, including 700 rejected compilations, with raw best
4546. Its useful edit moves the state-2 tpage store before the rotation-Z
assignment, within the same straight-line block and before the same calls.
Results moving parameter writes across states or calls were rejected.
Separating the sine division from its radial offset in states 3 and 4 lowers
the retained score to 4526. A local phase copy for state 4's radial-scale
calculation lowers it to **4491**. These changes add no pins, barriers,
conversion helpers or CPU ASM. Fresh compilation uses the same stock GCC,
maspsx and full-function weighted Levenshtein scorer; 2368 target and 2368
candidate instructions, still not a match.

Other audited hypotheses remain research-only:

- Twelve GPU prototype/conversion combinations: an int GetTPage return plus
  an ordinary u16 cast retains 4889 without gpuWord, but does not improve
  score or establish the original prototype. Retained declarations unchanged.
- Individual parameter-field unification: best 4889, all fields 13528.
- Four GTE variable-scope changes and four inlining variants: best 4889.
- Twenty-two empty-memory-barrier placements around parameter06 stores on
  the 4646 base: all worse, none retained.
- Combining the scale-expression split with a redundant scale copy is worse
  (4746); a named phase difference and reuse of an existing phase temporary
  both score 4526. Only the verified 4491 source is retained.

The darwine acceptance tree, synchronized through main commit e4b65a7c7,
passes `make -j32 verify`; log `match_inventory/scene-e19-4491-verify.log`.
The candidate remains outside production and the permuter run has stopped.

## Exhaustive draw-store ordering pass (4252)

The retained source now scores **4252**, freshly compiled and linked on
stock darwine GCC 2.7.2/maspsx. Target and candidate each contain 2368
instructions. Eight pins, eight barriers and two conversion helpers remain;
no new compiler constraints or CPU ASM were introduced.

A 120-second, 24-worker permuter run from 4491 completed 5206 iterations
(678 rejected compilations). Its best 4476 candidate swapped the state-2
palette/rotation-Z stores and introduced a neutral matrix pointer temporary.
Isolating the store swap retained 4476; the temporary was discarded.

All 720 permutations of the six independent pre-GetTPage assignments in
state 2 were then compiled and scored, yielding 4313. All 720 permutations
in each of the other ten equivalent setup blocks were also checked (7200
successful compilations). Eight blocks improved individually; the best
single-block result was 4284. All 256 combinations of those eight choices
were checked, and combining all eight yielded 4252. These reorderings stay
inside their original straight-line blocks, cross no calls or conditions,
and do not change any assigned values. The tables/selectors being read are
separate from the parameter globals being written.

Reproduction: `state2_order_4476.py`, `draw_orders_4313.py`, and
`combine_draw_orders.py`, with their named result directories and JSON
scores, in the existing darwine research directory. All searches finished.

Additional negative results, kept only as research artifacts:

- All 256 subsets of the eight existing register pins on the 4491 base
  compiled; none improved 4491 (`pin_subsets_4491/`).
- Sixteen volatile-store variants in draw states 0/1 all worsened 4491.
- Four explicit 64-bit multiply-high implementations of division by three
  were worse and added instructions; ordinary C division is retained.
- Three barriers, a volatile load and a conversion-helper variant around
  the draw-state load did not improve 4313; none was retained.

The production function remains original assembly. Main changes through
3826472cd were synchronized into the darwine acceptance tree, including the
updated source-policy scripts. `make -j32 verify` passes
(`match_inventory/scene-e19-4252-verify.log`).

## Multiply-high and compiler-option audit (4252 retained)

No candidate improvement in this pass. The canonical source was freshly
recompiled and rescored at 4252 after every experiment; no experimental
helper, flag, pin or barrier was retained.

The GCC 2.7.2 MIPS reference `smulsi3_highpart` pattern uses a logical
right shift of the signed 64-bit product. Explicit unsigned casts before
the shift still failed to select the desired short sequence in this source:
`high_logical_0..3` scored 5412, 5412, 5502 and 5787, adding instructions.
Union-field extraction probes selected mflo instead of mfhi and are invalid
for signed divide-by-three; they are not candidate alternatives.

Constraint-only HI/HILO extraction experiments can emit mfhi t1 using C
multiplication plus empty ASM constraints, but disturb other registers and
scheduling. A sign input dependency restores the local instruction order;
21 combinations of helper shape and its three call sites still fail to
improve the retained source (best 4272). Reusing/pinning the input and sign
also fails (4272..4312). These experiments are high_constraint_*, high_sign_*,
high_reuse_* and high_inputpin_* in the remote research directory. They
establish a possible compiler route to t1, not an accepted implementation.

Eight inline wrappers around the state-0/1 parameter stores are neutral when
loads remain inside the helper, and worse when the texture page is an
argument (4887..5427). No helper was kept.

Stock compiler probes from 4252:

- Disabling caller saves, strength reduction, GCSE, CSE follow-jumps or
  peepholes, and selecting O3: unchanged 4252.
- Disabling expensive optimizations: 7627; CSE skip-blocks: 13605.
- Disabling the first or second scheduling pass: 57322 or 16617; O1: 97790.

The 180-second, 24-worker permuter_draw_4252 run widened the expression/type
mutation set. It completed 8916 iterations (995 rejected compilations)
without improving 4252, and has stopped. Keep future work focused on source
lifetimes/representation rather than repeating these option/helper probes.

## Ring-radius register and scalar-lifetime audit (4237)

Pinning ringRadius to s1 reduces 4252 to **4237**, with the existing tied
radius barrier unchanged. Retail keeps 2000 in s1 across the two trig calls
and both ring-coordinate multiplies. The earlier renderParams value also
uses s1, but its final use precedes radius initialization; their live ranges
do not overlap. Candidate debt is now **nine pins, eight empty barriers,
two conversion helpers, no CPU ASM**. Target and candidate each still have
2368 instructions. This remains a nonzero, unintegrated candidate.

The radius pin was kept only after these source alternatives were checked:

- 32 scope/pin combinations for pageSelector, renderParams, ringRadius,
  loop index and ring angle; narrower setup scopes were neutral.
- 25 placements of index/angle guards with the radius pin; all index/angle
  constraints worsened the radius-only result. Eight follow-up input-only
  and pre-loop guard variants also failed to improve it.
- 30 combinations sharing four groups of non-overlapping arithmetic
  temporaries, and 812 directed merges of individual s32 temporary pairs;
  none improved the 4252 baseline. Ring products can share a temporary
  neutrally, but the cleanup is not retained in this matching pass.
- Fifteen timer-load forms in draw states 1/2: plain local temporaries and
  savedHeight are neutral; a0 constraints worsen the result.

Artifacts: ring_scope_*, ring_guard_4252_*, ring_input_guard_*, scalar_groups_*,
scalar_pairs_4252/ and phase_timer_* in the darwine research directory.
All probes finished. The final source is freshly compiled with the unchanged
stock compiler/assembler and full-function weighted Levenshtein scorer.

After syncing main through 04c8b6d86, `make -j32 verify` passes on darwine
(`match_inventory/scene-e19-4237-verify.log`).

## Aggregate representation and compiler identity audit (4237 retained)

No score reduction; the canonical C source and candidate debt are unchanged.
Every trial below was compiled on darwine with the same full-function scorer.

- Grouping each of the eleven MATRIX/VECTOR local pairs into a structure,
  individually or all together, preserves 4237 and the stack/instruction
  layout. The neutral aggregate source was used as a separate permuter seed:
  120 seconds, 24 workers, 5995 iterations, 720 rejected compilations, no
  improvement. `permuter_pairs_4237/` is stopped.
- Eighteen orders/forms chaining the three 0x40 parameter assignments in
  draw states 0/1 worsen the score (4632..5262).
- The existing matched FieldEng_Billboard/FieldEng_ShadedQuad sources use
  named GteMatrixWords transfers. Applying that representation here, with
  slot pointer/volatile-barrier variations, is neutral. Removing the matrix
  pointer pin with that representation gives 4277, still worse.
- Combining the GetTPage call with its OR/conversion, at each of eleven
  sites or all sites together, is neutral. Introducing explicit s32/u16
  palette-index locals at ten global-palette sites is also neutral.
- Splitting the later model height from earlier height uses, and guarding
  model phase/height in s1/s2, gives no improvement (best 4237).
- Plain register hints on nine important scalar variables, individually or
  all ordinary scalar declarations together, are neutral. No hints retained.

Compiler identity was checked directly: old-gcc/cc1 and psyq-gcc-2.7.2/cc1
in the darwine acceptance tree are identical static Linux i386 ELF binaries,
SHA-256 0359379289db8e3904b8ed3b25b422ca631c8d71ab2f543d533c39ed2967764f.
Thus the alternative directory is not a different compiler, and the current
host compiler is already 32-bit. A fresh stock native GCC 2.8.1 probe of the
current source scores 59417; the canonical 2.7.2 build was restored at 4237.

Artifacts: draw_pair_*, params_chain_*, gte_words_4237_*, gpu_combined_*,
model_scoped_*, palette_index_*, register_hint_* and stock281_4237.* in the
existing remote research directory. These negative results do not establish
that a match is impossible; avoid repeating the same representation probes.


## Split texture-page reads and draw-state fence (3397)

The first two draw-state setup blocks now read the texture page into a local
before writing palette/flags, then store that saved value to tpage. An empty
memory barrier separates the read from those writes. State 1 additionally
passes the saved page as an input operand; state 0 needs no register operand.
This changes 4237 to 3607. A third empty memory barrier after the two common
parameter writes keeps the state load at its retail position, giving **3397**.
No new register pins, register clobbers or CPU instructions are introduced.
Current debt is nine pins, eleven empty barriers and two inline conversions.

The table read and stores remain in the same straight-line setup, without
crossing a call or a conditional. The retained order follows the retail read
before palette/flag writes. This remains an unintegrated, nonzero candidate;
the complete function still requires semantic review and score zero.

All trials ran on darwine with stock GCC 2.7.2/maspsx and the full-function
weighted Levenshtein scorer. Merely splitting reads into locals (224 variants)
did not improve 4237. Adding fences at different read positions (280 variants)
found the useful boundary. Two rounds of 288 store-order permutations found
no further gain; 96 fence forms identified the simpler state-0 barrier, and
21 common-state fence placements found the final 210-point improvement.
Target and retained candidate both contain 2368 instructions.

Remote artifacts: split_page_4237/, split_page_guard_4237/,
page_orders_3687/, page_fence_3687/, page_orders_3607/, state_fence_3607/,
and base_3397.c under the existing scene_e19_80192F9C research directory.


## Negate before multiplying model rotation (2757)

Three Y rotations now use `(-D_800E27EC) * positive_constant` rather than
`D_800E27EC * negative_constant`. This follows the retail instruction order
and reduces 3397 to 3017 without a helper, pin or barrier. Exhaustive ordering
of the six setup writes in these three blocks (2160 variants) then found two
independent improvements: move the zero Z write before the tpage write in
the two -32 rotation blocks. Combined, these changes score **2757**.
The -48 block retains its existing write order. These are straight-line
arithmetic/store changes; no call or branch is crossed. Debt remains nine
pins, eleven empty barriers and two inline conversion helpers.

The 512 negative-rotation forms included direct signed/unsigned negation,
a temporary, an inline helper and empty constraints. The retained form is
ordinary C. Another 324 page-read/fence variants did not improve 2757.
The target and candidate still each contain 2368 instructions, with the
same stack frame. Only the full-function weighted Levenshtein score is
used to rank variants; the candidate is not yet matching or integrated.

Other closed searches: the 3397 draw permuter completed 6661 iterations
without improvement. Combined phase/scale/saved-height pin and helper
variants (128) did not improve 3397. Pinning a saved height to s5 could
score 3377 in an earlier 144-variant probe, but did not reproduce the retail
s7 allocation and was not retained; the ordinary-C rotation changes now
supersede that result. The 36 third-state setup fences were also neutral
or worse.

All compilation and scoring ran on darwine with stock GCC 2.7.2/maspsx.
Artifacts in the existing research directory: negative_rotation_3397/,
negative_orders_3017/, negative_page_2757/, model_live_3397/,
height_pins_3397/, state2_fences_3397/, permuter_draw_3397/ and base_2757.c.


## Third-state setup and per-block parameter fields (2141)

Moving the third draw state's intensity initialization before the parameter
pointer initialization reduces 2757 to 2537. The existing pins and barriers
are unchanged. This was selected from 216 setup-prefix order/guard variants.

Seven model setup blocks now write the flag via
`D_800F3368.parameter06`, and one of those also writes `D_800F3368.tpage`,
instead of using the equivalent standalone globals. These existing structure
members retain the original addresses and halfword widths. The parameter06
member is u16 whereas the standalone D_800F336E declaration is s16; these
changed stores write zero, whose representation is identical in both types.
Testing 77 per-block
field subsets and all 128 combinations of the improving subsets yielded
**2141**. This changes compiler scheduling, but does not yet reproduce every
retail store order. Both complete functions still contain 2368 instructions;
no production ASM has been replaced and no additional debt was introduced.

Closed probes: selector/phase initialization positions (120), separate flag
reads (64), flag reads combined with fences (216), volatile zero stores (77),
chained zero assignments (236), targeted zero-store dependencies (165), and
zero-store inline helpers (44) did not improve their respective retained
bases. All ran on darwine with the stock toolchain and full-function scorer.

Related division research remains unretained: 90 per-site HI constraint
variants did not improve 2757. Of 108 factor/work constraint variants, one
reproduced the first divide-by-three instruction sequence and scored 2747,
but added three local pins and three empty constraints for only ten points.
The simpler retained source now scores better overall. Whole-model lifetime
splits with phase/height pins (128 variants) also did not improve 2757.

Artifacts: state2_prefix_2757/, state2_selector_2537/, state2_flags_2537/,
state2_flag_fence_2537/, zero_stores_2537/, zero_chains_2537/,
zero_dependencies_2537/, zero_inline_2537/, draw_fields_2537/,
combine_fields_2537/, division_sites_2757/, division_factor_2757/,
model_live_2757/ and base_2141.c in the existing darwine research directory.


## Store order after parameter-field recovery (1207)

The seven blocks changed to structure members exposed new scheduling choices.
All 5040 permutations of their six setup writes were evaluated, followed by
128 combinations of the per-block improvements. The combined source scored
1557, down from 2141. Searching three earlier setup blocks jointly over flag
access form (global/member) and store order added 4320 trials; combining two
improving blocks reduced the full-function score to **1207**.

These changes keep the six writes in their original straight-line setup,
before the texture-page call. No call or branch is crossed. Two additional
flag writes now use the existing parameter06 member. There are no new pins,
barriers, helpers, volatile accesses or CPU instructions. The complete target
and candidate both contain 2368 instructions. Production still uses the
original assembly; 1207 is not a match.

The preceding 176 per-block combinations of palette read/write views and
flag/page field views did not improve 2141. Artifacts on darwine:
palette_views_2141/, field_orders_2141/, combine_orders_2141/,
early_fields_orders_1557/, combine_early_1557/ and base_1207.c.


## Separate sine result and third-state parameter fields (907)

The first draw-state sine result now has its own s32 local. This reproduces
the retail move/branch/delay slot before division by 32; alone it scored 1227
versus 1207. Combining that local with the initial third-state palette and
flag writes through D_800F3368 fixes the other displaced load-delay slot and
scores **917**. The prepared 128 combinations were run after SSH access was
restored. These changes add no pins, barriers or helpers. The flag write is
one, with the same representation in the standalone s16 and member u16.

A subsequent 144-variant timer-local probe found **907** by loading the timer
into a separate s32 local pinned to a0 before forming phase. It restores the
two retail register operands without adding an instruction or an empty
constraint. This introduces one pin, phaseTimer/a0 ($4); current debt is ten
pins, eleven empty barriers, and two inline conversion helpers. Ordinary
s32/s16/register locals did not produce this additional improvement.

Repeating the 216 prefix-order and 120 selector/phase-position variants on
the 917 source yielded no improvement. Both target and retained candidate
still contain 2368 instructions. Stock GCC 2.7.2 and maspsx, full-function
weighted Levenshtein scoring, and all compilations on darwine remain in use.
The candidate is not integrated or matching.

Remote artifacts: pending_state2_fields_1207/, state2_prefix_917/,
state2_selector_917/, state2_timer_917/, base_917.c and base_907.c.


## Defer radial-scale source initialization (787)

Moving the third draw state's radialScale = 0x1000 assignment immediately
after its first func_800CEE20 call makes GCC schedule the machine assignment
at the retail location before that call. The value is not an argument to the
call and has no intervening use; subsequent scale adjustment is unchanged.
Of 27 positions/guard forms, the retained ordinary assignment scored **787**,
down from 907, without additional debt. Fresh full-function scoring reports
2368 instructions for both target and candidate.

The remaining pointer-based matrix setup was searched over all six-store
orders and eight equivalent halfword pointer/member access combinations
(5760 variants), with no improvement. Another 144 initial-selector barrier
and operand-order variants also failed to improve 787 and were discarded.
Artifacts on darwine: state2_scale_907/, matrix_setup_787/,
selector_dependency_787/ and base_787.c. All searches finished; no matching
or production integration is claimed.
