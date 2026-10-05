# Battle_StepVictory — matched (2026-10-05)

Implemented in `src/main/battle/Battle_StepVictory.c` and enabled together with
its 32-byte jump table in the main manifest. The existing symbol name is kept.
The eight-stage transition fades two render objects, spawns effect 0x69,
advances the player animation, restores stats, and resets HUD color/UV bytes.

Stock native GCC 2.7.2 (`-O2 -G0 -funsigned-char -mips1 -mcpu=3000`) and
stock MASPSX (2.56 mode, `-G8 --use-comm-section`, forced assembler G0) produce
asm-differ Levenshtein **score 0**, **1616/1616 identical linked text bytes**,
and the exact **32-byte retail jump table**, verified on darwine.
Text: main offset `0x1B298`, address `0x8002AA98`, size `0x650`.
Table: main offset `0x10F0`, address `0x800108F0`.
Text SHA-256: `41a7819e6525ce1bfb44eab5c13953844f73dcff107f7b6370f38170e061bb39`.
The formatted, header-integrated source was recompiled as `promoted`, score 0.
The integrated `make -j32 verify` passed: the complete `main.exe` is identical
to retail. Source-policy, source-mapping and debt checks passed.

It uses shared BattleEntity, Combatant and RenderObjectEntity layouts; no
new opaque struct or raw entity offsets were needed. Interior HUD byte-symbol
declarations live in `pe1/battle_status.h`. The `variant_visible` address is
converted back to its containing render object with the shared field offset,
preserving the original interior-symbol relocation.

Debt: **nine pins and one empty pointer barrier**, shared goto tails, pinned
HUD constants, COMMON metadata for GP accesses, interior HUD symbols, and a
signed character view of D_8009D2A0 for retail's `lb`. No instruction ASM,
assembler/compiler modifications, or EABI. Four additional pins were removed
together without changing text or table bytes.

Research: `scratch/Battle_StepVictory` locally and
`/home/hasik/fx-search-archives/Battle_StepVictory` on darwine. The handwritten
C seed `base` scored 3530 (1608 bytes); `refine` reached 1080 (1616 bytes and
matching table); `refine2` matched; `minimal15` removed four redundant pins.
