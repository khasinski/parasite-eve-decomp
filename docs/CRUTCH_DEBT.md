# Crutch debt

_Matching crutches still to remove to reach clean C (GOAL.md). Read-only; regenerate with `make debt`. Watch these shrink._

**pins** = `register T x asm("$r")` · **barriers** = empty `asm("")` · **aliases** = `extern T x asm("sym")` · **asm_bodies** = real instructions · **directives** = `asm(".word ...")` · **gotos** · **include_asm** · **postpass**

| subsystem | pins | barriers | aliases | asm_bodies | directives | gotos | include_asm | postpass | dirty files |
|---|---|---|---|---|---|---|---|---|---|
| main/engine | 263 | 69 | 0 | 40 | 0 | 26 | 0 | 0 | 62 |
| main/main | 190 | 66 | 0 | 4 | 2 | 138 | 0 | 0 | 45 |
| main/psyq | 212 | 15 | 0 | 23 | 0 | 30 | 0 | 0 | 38 |
| main/gpu | 171 | 28 | 0 | 34 | 0 | 45 | 0 | 0 | 40 |
| main/akao | 137 | 35 | 0 | 2 | 0 | 20 | 0 | 0 | 33 |
| main/menu | 117 | 20 | 2 | 9 | 0 | 40 | 0 | 0 | 30 |
| main/render | 89 | 6 | 0 | 14 | 0 | 7 | 0 | 0 | 12 |
| main/task | 68 | 21 | 0 | 9 | 0 | 26 | 0 | 0 | 21 |
| main/memcard | 74 | 21 | 0 | 0 | 1 | 50 | 0 | 0 | 17 |
| main/item | 64 | 7 | 0 | 0 | 0 | 16 | 0 | 0 | 15 |
| main/cdrom | 38 | 22 | 7 | 3 | 0 | 7 | 0 | 0 | 15 |
| main/entity | 22 | 6 | 9 | 9 | 0 | 19 | 0 | 0 | 9 |
| main/math | 24 | 2 | 0 | 6 | 0 | 0 | 0 | 0 | 5 |
| main/battle | 18 | 4 | 0 | 4 | 0 | 6 | 0 | 0 | 10 |
| main/save | 14 | 7 | 2 | 2 | 0 | 4 | 0 | 0 | 8 |
| main/sys | 16 | 0 | 0 | 2 | 0 | 0 | 0 | 0 | 4 |
| main/util | 11 | 3 | 0 | 2 | 0 | 0 | 0 | 0 | 4 |
| main/obj | 7 | 0 | 0 | 7 | 0 | 0 | 0 | 0 | 1 |
| main/boot | 7 | 1 | 0 | 5 | 0 | 21 | 0 | 0 | 5 |
| main/anim | 8 | 3 | 0 | 0 | 0 | 0 | 0 | 0 | 4 |
| main/asset | 8 | 2 | 0 | 0 | 0 | 0 | 0 | 0 | 2 |
| main/pm | 6 | 1 | 0 | 1 | 0 | 0 | 0 | 0 | 2 |
| main/gte | 6 | 1 | 0 | 0 | 0 | 0 | 0 | 0 | 1 |
| main/aya | 5 | 0 | 0 | 0 | 0 | 5 | 0 | 0 | 1 |
| main/field | 3 | 2 | 0 | 0 | 0 | 1 | 0 | 0 | 1 |
| main/audio | 3 | 0 | 0 | 0 | 0 | 13 | 0 | 0 | 1 |
| main/time | 1 | 1 | 0 | 0 | 0 | 0 | 0 | 0 | 1 |
| main/overlay | 1 | 0 | 0 | 0 | 0 | 2 | 0 | 0 | 1 |
| main/scene | 1 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 1 |
| main/event | 0 | 0 | 0 | 0 | 0 | 4 | 0 | 0 | 0 |
| main/pad | 0 | 0 | 0 | 0 | 0 | 2 | 0 | 0 | 0 |
| overlays | 0 | 0 | 0 | 0 | 0 | 318 | 0 | 0 | 0 |
| **TOTAL** | 1584 | 343 | 20 | 176 | 3 | 800 | 0 | 0 | 389 |
