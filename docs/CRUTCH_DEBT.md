# Crutch debt

_Matching crutches still to remove to reach clean C (GOAL.md). Read-only; regenerate with `make debt`. Watch these shrink._

**pins** = `register T x asm("$r")` · **barriers** = empty `asm("")` · **aliases** = `extern T x asm("sym")` · **asm_bodies** = real instructions · **directives** = `asm(".word ...")` · **gotos** · **include_asm** · **postpass** · **externs_in_c** = declarations awaiting a subsystem header. Raw offset, pointer, field-macro, statement-expression, unknown-field, and declaration-override columns track semantic/typing scaffolding.

| subsystem | byte_pointer_arithmetic | raw_offset_dereferences | pointer_integer_casts | field_macros | pins | barriers | aliases | asm_bodies | directives | gotos | include_asm | postpass | statement_expressions | unknown_fields | declaration_overrides | externs_in_c | dirty files |
|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
| overlays | 68 | 1790 | 16 | 0 | 0 | 0 | 0 | 0 | 0 | 318 | 0 | 0 | 0 | 52 | 0 | 2681 | 1174 |
| main/engine | 31 | 723 | 3 | 0 | 118 | 51 | 0 | 40 | 0 | 26 | 0 | 0 | 0 | 0 | 0 | 593 | 168 |
| main/main | 58 | 145 | 79 | 303 | 102 | 33 | 0 | 2 | 2 | 117 | 0 | 0 | 0 | 12 | 0 | 490 | 50 |
| main/menu | 72 | 92 | 40 | 202 | 34 | 13 | 0 | 5 | 0 | 34 | 0 | 0 | 0 | 41 | 0 | 480 | 141 |
| main/akao | 17 | 169 | 3 | 0 | 63 | 17 | 0 | 2 | 0 | 16 | 0 | 0 | 0 | 34 | 0 | 220 | 90 |
| main/gpu | 27 | 27 | 17 | 41 | 68 | 19 | 0 | 18 | 0 | 38 | 0 | 0 | 0 | 0 | 0 | 290 | 67 |
| main/psyq | 9 | 38 | 2 | 2 | 83 | 14 | 0 | 15 | 0 | 13 | 0 | 0 | 3 | 10 | 0 | 223 | 76 |
| main/render | 19 | 88 | 21 | 141 | 32 | 3 | 0 | 14 | 0 | 7 | 0 | 0 | 0 | 0 | 0 | 72 | 20 |
| main/memcard | 9 | 42 | 5 | 0 | 23 | 14 | 0 | 0 | 1 | 24 | 0 | 0 | 0 | 56 | 0 | 190 | 54 |
| main/task | 2 | 84 | 1 | 6 | 53 | 19 | 0 | 8 | 0 | 22 | 0 | 0 | 0 | 15 | 0 | 138 | 67 |
| main/battle | 32 | 119 | 1 | 7 | 9 | 2 | 0 | 2 | 0 | 4 | 0 | 0 | 0 | 3 | 0 | 108 | 34 |
| main/entity | 9 | 59 | 15 | 0 | 21 | 4 | 9 | 9 | 0 | 17 | 0 | 0 | 0 | 5 | 0 | 115 | 51 |
| main/item | 5 | 0 | 6 | 8 | 24 | 6 | 0 | 0 | 0 | 16 | 0 | 0 | 0 | 0 | 0 | 196 | 40 |
| main/cdrom | 3 | 14 | 2 | 25 | 15 | 18 | 7 | 3 | 0 | 7 | 0 | 0 | 0 | 0 | 0 | 111 | 33 |
| main/boot | 2 | 11 | 4 | 0 | 7 | 1 | 0 | 5 | 0 | 21 | 0 | 0 | 0 | 0 | 0 | 80 | 11 |
| main/save | 2 | 3 | 0 | 0 | 2 | 4 | 1 | 1 | 0 | 4 | 0 | 0 | 0 | 0 | 0 | 93 | 17 |
| main/anim | 4 | 0 | 18 | 31 | 8 | 3 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 4 |
| main/pm | 0 | 30 | 1 | 1 | 6 | 1 | 0 | 1 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 20 | 7 |
| main/sys | 1 | 0 | 1 | 0 | 6 | 0 | 0 | 2 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 36 | 8 |
| main/util | 1 | 0 | 0 | 0 | 3 | 3 | 0 | 2 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 29 | 5 |
| main/field | 3 | 0 | 4 | 19 | 3 | 2 | 0 | 0 | 0 | 1 | 0 | 0 | 0 | 0 | 0 | 2 | 1 |
| main/table | 8 | 10 | 1 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 14 | 6 |
| main/obj | 8 | 5 | 0 | 0 | 7 | 0 | 0 | 7 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 2 | 5 |
| main/aya | 5 | 4 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 4 | 0 | 0 | 0 | 2 | 0 | 16 | 6 |
| main/overlay | 0 | 4 | 4 | 0 | 1 | 0 | 0 | 0 | 0 | 2 | 0 | 0 | 0 | 0 | 0 | 17 | 5 |
| main/asset | 3 | 7 | 1 | 0 | 8 | 2 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 4 | 5 |
| main/scene | 0 | 1 | 1 | 13 | 1 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 6 | 4 |
| main/math | 1 | 0 | 0 | 0 | 11 | 0 | 0 | 6 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 3 |
| main/audio | 0 | 0 | 4 | 3 | 2 | 0 | 0 | 0 | 0 | 13 | 0 | 0 | 0 | 0 | 0 | 7 | 3 |
| main/time | 3 | 0 | 0 | 0 | 1 | 1 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 3 | 2 |
| main/pad | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 2 | 0 | 0 | 0 | 0 | 0 | 5 | 5 |
| main/event | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 4 | 0 | 0 | 0 | 0 | 0 | 3 | 2 |
| main/gte | 0 | 0 | 0 | 0 | 1 | 1 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 1 |
| **TOTAL** | 402 | 3465 | 250 | 802 | 712 | 231 | 17 | 142 | 3 | 710 | 0 | 0 | 3 | 230 | 0 | 6244 | 2165 |
