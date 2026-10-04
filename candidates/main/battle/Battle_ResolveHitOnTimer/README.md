# Battle_ResolveHitOnTimer — WIP, not matched

Retail main function: 0x800236E8, 1836 bytes (file 0x13EE8..0x14614).
Target SHA-256: 3b52c1cb0be9f1f5d206297387a68e79a3ed39cc950d9c2e5e0ffceac283a0a4.

Current candidate: linked asm-differ score 5130, 1828 bytes; **not identical**.
The manifest remains ASM. The source is a research checkpoint, not production.

Compile/permutation work runs only on `ssh darwine`, using stock GCC 2.7.2 and
stock MASPSX, profile 2.56. Search workspace:
`/home/hasik/fx-search-archives/Battle_ResolveHitOnTimer`.
Acceptance checkout: `/home/hasik/fx-search-archives/scene_e20_acceptance`.
`result.c` is this candidate; `result.linked.json` and `result.linked.diff`
record the comparison against the retail bytes. No permuter is currently running
for this function. Do not reuse earlier panel-renderer jobs; they were stopped.

Reproduce in the remote search workspace:

```
python3 compile_default_g0.py result
/home/hasik/fx-search-archives/scene_e20_acceptance/.venv/bin/python evaluate.py result
```

The experimental compiler driver omits `--dont-force-G0`: MASPSX receives -G8
and uses COMMON metadata to emit explicit GP loads/stores, then invokes GNU as
with its stock default -G0. This keeps address-taking of D_8009CE54 absolute while
its byte accesses stay GP-relative, as in retail. The production wrapper currently
adds --dont-force-G0 automatically for -G8, so this mode still needs explicit
wrapper support before any promotion. Neither GCC nor MASPSX was modified.
COMMON symbols are resolved to the original addresses by the comparison linker;
this candidate must not allocate duplicate global storage when promoted.

Useful findings:

- EnemyCombatant +0x9F is the stealable item ID; successful transfer clears it.
- Enemy flags bits 13..14 are the action phase; 15..17 hold the reaction and
  18..19 receive action attackWord bits 20..21. A bitfield view improves the score.
- The cone attack needs signed 16-bit angle bounds, widened before the traversal,
  and an independently cached upper-range predicate. The current source preserves
  those comparisons explicitly; its register allocation is still different.
- Capturing the result actor/status before the shared hit/miss paths keeps the
  initial steal-action path distinct, matching the original control-flow layout.
- Remaining differences include loot-result load/store scheduling, repeated
  bitfield updates, signed reaction loads, and cone-loop register allocation.

Progress: baseline 11510 -> angular flow 6765 -> bitfields 5970 -> current 5130.
Experimental hard-pinning all cone locals worsens allocation and spills extra
saved registers; do not treat those variants as improvements.
