# Decompilation progress

_Regenerate with `make progress`._

A translation unit counts as decompiled when it has no INCLUDE_ASM
and no inline/register assembly. Pure C is the target. Code bytes cover function subsegments
only (baked data carriers are excluded). Every built binary is
byte-identical to retail (`make check`, `make overlay-check-all`).

**Headline: 901752/3759984 code bytes (24.0%) are decompiled.**
Function coverage is secondary: 8511/12215 (69.7%).

| Binary | Functions | % | Code bytes | % |
|---|---:|---:|---:|---:|
| `SLUS_006.62 (main)` | 1291/2555 | 50.5% | 137588/843348 | 16.3% |
| `boot_display` | 1/16 | 6.2% | 240/10240 | 2.3% |
| `fx_common` | 0/66 | 0.0% | 0/53248 | 0.0% |
| `fx_field` | 1/3 | 33.3% | 944/4096 | 23.0% |
| `menu_memcard` | 5/45 | 11.1% | 1384/45056 | 3.1% |
| `render_clip` | 0/2 | 0.0% | 0/2048 | 0.0% |
| `scene_e01` | 11/20 | 55.0% | 760/12768 | 6.0% |
| `scene_e02` | 69/96 | 71.9% | 4672/25732 | 18.2% |
| `scene_e03` | 46/60 | 76.7% | 2916/15756 | 18.5% |
| `scene_e04` | 15/32 | 46.9% | 1080/12176 | 8.9% |
| `scene_e05` | 15/32 | 46.9% | 1080/12176 | 8.9% |
| `scene_e06` | 50/65 | 76.9% | 2800/15756 | 17.8% |
| `scene_e07` | 50/65 | 76.9% | 2800/15756 | 17.8% |
| `scene_e08` | 108/161 | 67.1% | 8148/38540 | 21.1% |
| `scene_e09` | 59/85 | 69.4% | 3612/23584 | 15.3% |
| `scene_e10` | 59/85 | 69.4% | 3612/23584 | 15.3% |
| `scene_e11` | 19/28 | 67.9% | 1812/10776 | 16.8% |
| `scene_e12` | 19/28 | 67.9% | 1812/10776 | 16.8% |
| `scene_e13` | 19/28 | 67.9% | 1812/10776 | 16.8% |
| `scene_e14` | 18/25 | 72.0% | 1784/7472 | 23.9% |
| `scene_e18` | 46/69 | 66.7% | 2916/20212 | 14.4% |
| `scene_e19` | 57/75 | 76.0% | 3652/16308 | 22.4% |
| `scene_e19_2` | 0/2 | 0.0% | 0/25196 | 0.0% |
| `scene_e20` | 0/2 | 0.0% | 0/6144 | 0.0% |
| `scene_e22` | 36/68 | 52.9% | 2472/43008 | 5.7% |
| `scene_e24` | 50/66 | 75.8% | 2708/16724 | 16.2% |
| `scene_e25` | 50/66 | 75.8% | 2708/16724 | 16.2% |
| `scene_e26` | 46/59 | 78.0% | 2916/13568 | 21.5% |
| `scene_e27` | 46/60 | 76.7% | 2916/15756 | 18.5% |
| `sys_reset` | 17/19 | 89.5% | 1620/2048 | 79.1% |
| `room overlays (x162)` | 6308/8232 | 76.6% | 700988/2390632 | 29.3% |
| **total** | **8511/12215** | **69.7%** | **901752/3759984** | **24.0%** |
