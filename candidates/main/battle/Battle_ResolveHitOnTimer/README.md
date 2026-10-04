# Battle_ResolveHitOnTimer — matched and promoted

Production source: `src/main/battle/Battle_ResolveHitOnTimer.c`.
The manifest now selects C for 0x13EE8..0x14614 (VRAM 0x800236E8).

Validation on darwine with stock native GCC 2.7.2 and stock MASPSX 2.56:

- Linked asm-differ score **0**, all **1836 bytes identical** to retail.
- SHA-256: `3b52c1cb0be9f1f5d206297387a68e79a3ed39cc950d9c2e5e0ffceac283a0a4`.
- Full `make -j32 verify`: `build/USA/main.exe: OK`.
- Matching debt after removal trials: 8 pins, 5 empty barriers, no symbol
  aliases or instruction ASM. Recorded in the crutch-debt baseline.

`MASPSX_FORCE_G0: 1` tells the project compiler wrapper to retain MASPSX's
stock forced-G0 assembler mode. COMMON metadata still produces explicit GP
loads/stores, while GNU as keeps address-taking absolute. Existing retail
symbols provide the COMMON storage. Neither GCC nor MASPSX was modified.

The repeated hit-flag updates need a cached flag word and active combatant,
plus a barrier before reading the reaction. The chained reaction is `s16`.
The cone traversal caches the player pointer and keeps signed angle bounds
in the retail saved registers. Capturing the enemy core before the item
result stores reproduces the steal path's aliasing and instruction ordering.

Research artifacts remain on darwine at
`/home/hasik/fx-search-archives/Battle_ResolveHitOnTimer`.
`last1.c` was the first score-zero candidate; production removes redundant
pins/barriers and aliases, names locals, and uses unsigned left shifts.
No permuter was needed for this match; no job was left running.
