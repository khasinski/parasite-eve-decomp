# Missing scene_e19 function

The progress report's `overlays/scene_e19_2/scene_pre` entry is the tail of
`func_80192F9C`, not an independent function at `0x8018EFF0`. This function must
remain outstanding for the 100% goal. Its Levenshtein score is not yet measured.

Evidence was checked directly against `disc/extracted/PE.IMG`, with disassembly
on darwine. Offsets below are relative to sector 98589 (2048-byte sectors), whose
mapped address is `0x8018EFE8`.

| Full-scene range | Address range (end exclusive) | Extraction before the fix |
| --- | --- | --- |
| `0x3FB4..0x4000` | `0x80192F9C..0x80192FE8` | Last 76 bytes of scene_e19, incorrectly marked data |
| `0x4000..0x6000` | `0x80192FE8..0x80194FE8` | Missing sectors 98597..98600: 8192 bytes |
| `0x6000..0x64B4` | `0x80194FE8..0x8019549C` | scene_e19_2's 1204-byte scene_pre prefix, incorrectly mapped at 0x8018EFF0 |

The complete function is 9472 bytes (`0x2500`). The preceding function returns
at `0x80192F94`, with its delay slot at `0x80192F98`. The missing function starts
with `addiu sp,sp,-0x330` and ends with `jr ra` at `0x80195494` and its delay slot
at `0x80195498`. There is no intervening stack-allocation prologue. The next
already implemented function starts at `0x8019549C`.

Two bounded five-way switches provide additional control-flow evidence:

| Table | Target addresses |
| --- | --- |
| `0x8018F1E4` | `801931E8, 80193280, 801932AC, 801932D8, 8019352C` |
| `0x8018F1FC` | `80193760, 80193884, 80193C5C, 80194158, 80194C1C` |

To reproduce the complete probe without changing the production assets:

```sh
dd if=disc/extracted/PE.IMG of=scene_e19_full_probe.bin bs=2048 skip=98589 count=25
```

On darwine, disassemble with spimdisasm `singleFileDisasm`, passing
`--start 0x3FB4 --end 0x64B4 --vram 0x80192F9C --endian little --abi O32
--arch-level MIPS1 --instr-category r3000gte --compiler GCC --gp 0x8009CD70`.
Include the two jump tables and label their destinations before using m2c.

Research artifacts are in `scratch/match_inventory/` locally and
`/home/hasik/fx-search-archives/match_inventory/` on darwine. The corrected little-endian m2c recovery now produces a compilable research
candidate in `candidates/overlays/scene_e19_func_80192F9C/`. Its linked weighted
Levenshtein score is 11630; it is not integrated or a claimed match.

Production extraction now splits at full-scene offset `0x3FB4`: scene_e19
ends there, and scene_e19_2 starts there at `0x80192F9C`. Its next segment
starts at offset `0x2500`, retaining the original address `0x8019549C`.
All existing C function addresses remain unchanged. The complete outstanding
function is represented by one original-assembly subsegment, `func_80192F9C`.

CI assets are pinned to `c38afa7fead3c4dc0e894ce0c983b7d5e7544d4b` in the
private assets repository. Earlier source revisions keep their previous asset
pin. The extraction comments can regenerate both slices using the existing
`tools/scripts/extract_overlay_config_target.py` helper.

Both corrected overlays pass `make overlay-check` on darwine. This verifies
extraction and build integration, not a C match: the recovered function is
still represented by original assembly; the nonzero candidate score does not
qualify it as matched.
