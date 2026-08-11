# Decompilation progress

_Regenerate with `make progress`._

A translation unit counts as decompiled when it has no INCLUDE_ASM
and no inline/register assembly. Pure C is the target. Code bytes cover function subsegments
only (baked data carriers are excluded). Every built binary is
byte-identical to retail (`make check`, `make overlay-check-all`).

**Headline: 866860/3759984 code bytes (23.1%) are decompiled.**
Function coverage is secondary: 8432/12248 (68.8%).

| Binary | Functions | % | Code bytes | % |
|---|---:|---:|---:|---:|
| `SLUS_006.62 (main)` | 1259/2555 | 49.3% | 124776/843348 | 14.8% |
| `boot_display` | 1/16 | 6.2% | 240/10240 | 2.3% |
| `fx_common` | 0/66 | 0.0% | 0/53248 | 0.0% |
| `fx_field` | 1/3 | 33.3% | 944/4096 | 23.0% |
| `menu_memcard` | 6/47 | 12.8% | 868/45056 | 1.9% |
| `render_clip` | 0/2 | 0.0% | 0/2048 | 0.0% |
| `scene_e01` | 12/21 | 57.1% | 760/12768 | 6.0% |
| `scene_e02` | 67/96 | 69.8% | 4400/25732 | 17.1% |
| `scene_e03` | 44/60 | 73.3% | 2644/15756 | 16.8% |
| `scene_e04` | 15/32 | 46.9% | 1080/12176 | 8.9% |
| `scene_e05` | 15/32 | 46.9% | 1080/12176 | 8.9% |
| `scene_e06` | 48/65 | 73.8% | 2528/15756 | 16.0% |
| `scene_e07` | 48/65 | 73.8% | 2528/15756 | 16.0% |
| `scene_e08` | 106/161 | 65.8% | 7876/38540 | 20.4% |
| `scene_e09` | 57/85 | 67.1% | 3340/23584 | 14.2% |
| `scene_e10` | 57/85 | 67.1% | 3340/23584 | 14.2% |
| `scene_e11` | 19/29 | 65.5% | 1568/10776 | 14.6% |
| `scene_e12` | 19/29 | 65.5% | 1568/10776 | 14.6% |
| `scene_e13` | 19/29 | 65.5% | 1568/10776 | 14.6% |
| `scene_e14` | 18/26 | 69.2% | 1540/7472 | 20.6% |
| `scene_e18` | 44/69 | 63.8% | 2644/20212 | 13.1% |
| `scene_e19` | 55/75 | 73.3% | 3380/16308 | 20.7% |
| `scene_e19_2` | 0/2 | 0.0% | 0/25196 | 0.0% |
| `scene_e20` | 0/2 | 0.0% | 0/6144 | 0.0% |
| `scene_e22` | 34/68 | 50.0% | 2200/43008 | 5.1% |
| `scene_e24` | 48/66 | 72.7% | 2436/16724 | 14.6% |
| `scene_e25` | 48/66 | 72.7% | 2436/16724 | 14.6% |
| `scene_e26` | 44/59 | 74.6% | 2644/13568 | 19.5% |
| `scene_e27` | 44/60 | 73.3% | 2644/15756 | 16.8% |
| `sys_reset` | 11/19 | 57.9% | 648/2048 | 31.6% |
| `room overlays (x162)` | 6293/8258 | 76.2% | 685180/2390632 | 28.7% |
| **total** | **8432/12248** | **68.8%** | **866860/3759984** | **23.1%** |
