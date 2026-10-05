# Particle-chain matrix composition candidate

`func_800C5538`, 1288 retail bytes at 0x800C5538. WIP, not a match and not
part of the production build. The accompanying C file replaces this function
in `src/main/engine/FieldEng_ParticleChainFlow.c`; the strip renderer stays unchanged.

The legacy `gte_CompMatrix` expansion has been replaced by C loads, stores,
packing and individual GTE instruction macros. Stock native GCC 2.7.2 and
stock MASPSX 2.56 were run exclusively on darwine, without EABI or tool patches.

Linked asm-differ score: **35**. Candidate and retail function sizes are both
1288 bytes. Verification included the complete 2424-byte TU; the three differing
rows are all in this function, and the following strip renderer is unchanged.
The existing production baseline scores zero and is retail-identical.

Remaining diff (retail | candidate):

```text
800c5634 addiu s4,sp,0x30 | addiu s1,s0,0x24
800c563c addiu s1,s0,0x24 | addiu s4,sp,0x30
800c5a10 bnez v0,800c5640 | bnez v0,800c5638
```

The arithmetic and GTE sequence match. GCC retains the two matrix-address
initializations inside the loop; retail initializes them once before the loop.
The candidate has five pins and ten empty constraints, prospective debt only.
Pins on the record and loop index proved unnecessary and have been removed.
Other pins and constraints have not yet been minimized.

Research: `scratch/particle_chain/` and
`/home/hasik/fx-search-archives/particle_chain/` on darwine. `reordered.c` is the
full TU reconstructed from this candidate and the unchanged production strip
renderer; `reordered.linked.json` and `reordered.linked.diff` record its evaluation.
Initial expansion scored 1709; aliases and pins reduced this to 45. Moving the
address setup before the loop scores 120 because GCC schedules it before the
matrix copy. Explicit entry guards / do-while forms change the stack frame or
register allocation and did not improve the candidate. 128 combinations of
address aliases, pins and original call arguments also failed to improve 45.
No permuter is running for this candidate.

A 90-second permuter run on darwine with 24 workers completed 23,472 iterations
(2,512 compile errors). Its improvement was solely swapping the two pointer
initializations, interpreted back into this macro-based source and independently
verified at linked score 35. Internal permuter scores were 35 initially and 30
for the improvement; those are not the linked asm-differ scores above.

`permuter_function/` contains the valid run. Its target ELF has a text section
restricted to the function's 1288 bytes. Earlier runs in `permuter/` and
`permuter_round2/` erroneously included the following renderer in their target;
their results are discarded. Adding a second symbol alone did not fix that
because this permuter scores the whole target text section. All workers stopped.

32 constraint variants and their reversed-initialization counterparts were also
evaluated. One variant (`constraint14.c`) needs four pins instead of five, but
scores 45; the retained five-pin candidate has the better score of 35.
