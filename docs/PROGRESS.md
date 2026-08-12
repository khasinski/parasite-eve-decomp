# Decompilation progress

_Regenerate with `make progress`._

A translation unit counts as decompiled when it has no INCLUDE_ASM
and no inline/register assembly. Pure C is the target. Code bytes cover function subsegments
only (baked data carriers are excluded). Every built binary is
byte-identical to retail (`make check`, `make overlay-check-all`).

**Headline: 1258884/3759940 code bytes (33.5%) are decompiled.**
Function coverage is secondary: 8756/11771 (74.4%).

| Binary | Functions | % | Code bytes | % |
|---|---:|---:|---:|---:|
| `SLUS_006.62 (main)` | 1278/2555 | 50.0% | 129228/843348 | 15.3% |
| `boot_display` | 1/16 | 6.2% | 240/10240 | 2.3% |
| `fx_common` | 20/65 | 30.8% | 2328/53204 | 4.4% |
| `fx_field` | 1/3 | 33.3% | 944/4096 | 23.0% |
| `menu_memcard` | 5/45 | 11.1% | 1384/45056 | 3.1% |
| `render_clip` | 0/2 | 0.0% | 0/2048 | 0.0% |
| `scene_e01` | 10/19 | 52.6% | 760/12768 | 6.0% |
| `scene_e02` | 68/95 | 71.6% | 4672/25732 | 18.2% |
| `scene_e03` | 46/60 | 76.7% | 2916/15756 | 18.5% |
| `scene_e04` | 15/29 | 51.7% | 1716/12176 | 14.1% |
| `scene_e05` | 15/29 | 51.7% | 1716/12176 | 14.1% |
| `scene_e06` | 50/65 | 76.9% | 2800/15756 | 17.8% |
| `scene_e07` | 50/65 | 76.9% | 2800/15756 | 17.8% |
| `scene_e08` | 110/161 | 68.3% | 8536/38540 | 22.1% |
| `scene_e09` | 59/84 | 70.2% | 3776/23584 | 16.0% |
| `scene_e10` | 59/84 | 70.2% | 3776/23584 | 16.0% |
| `scene_e11` | 19/28 | 67.9% | 1812/10776 | 16.8% |
| `scene_e12` | 19/28 | 67.9% | 1812/10776 | 16.8% |
| `scene_e13` | 19/28 | 67.9% | 1812/10776 | 16.8% |
| `scene_e14` | 18/25 | 72.0% | 1784/7472 | 23.9% |
| `scene_e18` | 46/69 | 66.7% | 2916/20212 | 14.4% |
| `scene_e19` | 57/74 | 77.0% | 3704/16308 | 22.7% |
| `scene_e19_2` | 0/2 | 0.0% | 0/25196 | 0.0% |
| `scene_e20` | 0/2 | 0.0% | 0/6144 | 0.0% |
| `scene_e22` | 37/68 | 54.4% | 3048/43008 | 7.1% |
| `scene_e24` | 50/66 | 75.8% | 2708/16724 | 16.2% |
| `scene_e25` | 50/66 | 75.8% | 2708/16724 | 16.2% |
| `scene_e26` | 46/59 | 78.0% | 2916/13568 | 21.5% |
| `scene_e27` | 46/60 | 76.7% | 2916/15756 | 18.5% |
| `sys_reset` | 17/19 | 89.5% | 1620/2048 | 79.1% |
| `room overlays (x162)` | 6545/7800 | 83.9% | 1061536/2390632 | 44.4% |
| **total** | **8756/11771** | **74.4%** | **1258884/3759940** | **33.5%** |
