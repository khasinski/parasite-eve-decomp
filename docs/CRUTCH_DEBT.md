# Crutch debt

_Matching crutches still to remove to reach clean C (GOAL.md). Read-only; regenerate with `make debt`. Watch these shrink._

**pins** = `register T x asm("$r")` · **barriers** = empty `asm("")` · **aliases** = `extern T x asm("sym")` · **asm_bodies** = real instructions · **directives** = `asm(".word ...")` · **gotos** · **include_asm** · **postpass**

| subsystem | pins | barriers | aliases | asm_bodies | directives | gotos | include_asm | postpass | dirty files |
|---|---|---|---|---|---|---|---|---|---|
| main/engine | 118 | 53 | 0 | 40 | 0 | 26 | 0 | 0 | 62 |
| main/main | 131 | 39 | 0 | 4 | 2 | 138 | 0 | 0 | 45 |
| main/gpu | 96 | 20 | 0 | 34 | 0 | 45 | 0 | 0 | 39 |
| main/psyq | 107 | 14 | 0 | 23 | 0 | 30 | 0 | 0 | 36 |
| main/akao | 86 | 25 | 0 | 2 | 0 | 20 | 0 | 0 | 33 |
| main/menu | 63 | 20 | 2 | 9 | 0 | 40 | 0 | 0 | 29 |
| main/task | 54 | 19 | 0 | 9 | 0 | 26 | 0 | 0 | 21 |
| main/render | 40 | 6 | 0 | 14 | 0 | 7 | 0 | 0 | 11 |
| main/memcard | 38 | 17 | 0 | 0 | 1 | 50 | 0 | 0 | 17 |
| main/cdrom | 15 | 18 | 7 | 3 | 0 | 7 | 0 | 0 | 15 |
| main/entity | 21 | 4 | 9 | 9 | 0 | 19 | 0 | 0 | 9 |
| main/item | 28 | 6 | 0 | 0 | 0 | 16 | 0 | 0 | 14 |
| main/math | 16 | 2 | 0 | 6 | 0 | 0 | 0 | 0 | 5 |
| main/battle | 9 | 2 | 0 | 4 | 0 | 6 | 0 | 0 | 9 |
| main/save | 5 | 6 | 2 | 2 | 0 | 4 | 0 | 0 | 7 |
| main/obj | 7 | 0 | 0 | 7 | 0 | 0 | 0 | 0 | 1 |
| main/boot | 7 | 1 | 0 | 5 | 0 | 21 | 0 | 0 | 5 |
| main/anim | 8 | 3 | 0 | 0 | 0 | 0 | 0 | 0 | 4 |
| main/asset | 8 | 2 | 0 | 0 | 0 | 0 | 0 | 0 | 2 |
| main/pm | 6 | 1 | 0 | 1 | 0 | 0 | 0 | 0 | 2 |
| main/sys | 6 | 0 | 0 | 2 | 0 | 0 | 0 | 0 | 4 |
| main/util | 3 | 3 | 0 | 2 | 0 | 0 | 0 | 0 | 4 |
| main/field | 3 | 2 | 0 | 0 | 0 | 1 | 0 | 0 | 1 |
| main/aya | 3 | 0 | 0 | 0 | 0 | 5 | 0 | 0 | 1 |
| main/audio | 2 | 0 | 0 | 0 | 0 | 13 | 0 | 0 | 1 |
| main/gte | 1 | 1 | 0 | 0 | 0 | 0 | 0 | 0 | 1 |
| main/time | 1 | 1 | 0 | 0 | 0 | 0 | 0 | 0 | 1 |
| main/overlay | 1 | 0 | 0 | 0 | 0 | 2 | 0 | 0 | 1 |
| main/scene | 1 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 1 |
| main/event | 0 | 0 | 0 | 0 | 0 | 4 | 0 | 0 | 0 |
| main/pad | 0 | 0 | 0 | 0 | 0 | 2 | 0 | 0 | 0 |
| overlays | 0 | 0 | 0 | 0 | 0 | 318 | 0 | 0 | 0 |
| **TOTAL** | 884 | 265 | 20 | 176 | 3 | 800 | 0 | 0 | 381 |
