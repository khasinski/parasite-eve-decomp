# parasite-eve-decomp

Work-in-progress matching decompilation of the North American PlayStation release
of Parasite Eve, target `SLUS-006.62`.

This repository is source-only. It does not contain disc images, extracted game
assets, FMV, memory cards, Sony PSYQ SDK files, compiler binaries, or generated
split output. Contributors must provide their own legally obtained copy of the
game and local toolchains.

## Target

| Field | Value |
|---|---|
| Game | Parasite Eve, USA Disc 1 |
| Executable | `SLUS_006.62` |
| Target path | `assets/USA/main.exe` |
| SHA-1 | `452fb033f2eaa4b18aa20a5bca60b8125af3a37b` |
| Entry PC | `0x80072534` |
| Text VRAM | `0x80010000..0x801FE000` |

## Layout

- `configs/USA/` - splat config, symbols, relocs, checksum.
- `tools/scripts/` - small project-specific build and analysis helpers.
- `src/main/` - matching C translation units that build without local post-build
  rewrite hooks.
- `include/` - project headers used when C is added.

Generated directories such as `asm/`, `linkers/`, `build/`, `assets/`, `disc/`,
and `extracted_assets/` are intentionally ignored.

## Progress

The current public baseline keeps every main executable function that builds
without local rewrite hooks as source, and leaves the remaining functions as
regenerated split assembly.

| Metric | Count |
|---|---:|
| C subsegments | 1501 |
| ASM subsegments | 438 |
| Pad subsegments | 115 |

That is about 77% C coverage by main executable code subsegment count. Some C
files still contain technical inline assembly or m2c compatibility macros; files
that required local post-build rewriting are intentionally excluded from this
release baseline.

## Toolchain

The current baseline rebuilds from regenerated split assembly and does not need
the proprietary PSYQ compiler. It expects `mipsel-none-elf-as`,
`mipsel-none-elf-ld`, and `mipsel-none-elf-objcopy` on `PATH`.

When adding matching C, the helper compiler wrapper can use these default local
paths:

```text
sdk/psyq-4.0/COMPILER/CPPPSX.EXE
sdk/psyq-4.0/COMPILER/CC1PSX.EXE
sdk/psyq-4.0/PSX/INCLUDE/
tools/wibo/wibo
tools/maspsx/maspsx.py
```

or with environment variables:

```sh
export PE_WIBO=/path/to/wibo
export PE_CPPPSX_EXE=/path/to/CPPPSX.EXE
export PE_CC1_BUILD0001_EXE=/path/to/CC1PSX.EXE
export PE_PSYQ_INCLUDE=/path/to/PSX/INCLUDE
export PE_MASPSX=/path/to/maspsx.py
export PE_CC1=/path/to/custom/cc1-wrapper      # optional
```

The default compiler path runs `CPPPSX.EXE` and `CC1PSX.EXE` through `wibo`.

## Quick Start

```sh
python3 -m venv .venv
.venv/bin/pip install -r requirements.txt

mkdir -p assets/USA
cp /path/to/your/SLUS_006.62 assets/USA/main.exe
shasum -a 1 assets/USA/main.exe

make check
```

Expected SHA-1 for `assets/USA/main.exe`:
`452fb033f2eaa4b18aa20a5bca60b8125af3a37b`.

`make build` regenerates `asm/` and `linkers/` from the user-supplied executable,
then compiles and links `build/USA/main.exe`. `make check` runs the same build
and verifies the SHA-1. Exact matching builds require local user-supplied inputs
and regenerated artefacts. Keep compiler, assembler, SDK, and game data outside
the repository.

## CI

Public GitHub Actions cannot run the full matching build because the repository
does not include the original game executable. The public CI job runs:

```sh
make ci
```

This checks source-only invariants: Python syntax, YAML parsing, absence of
tracked game/build artefacts, and absence of stale local-only rewrite hooks. Run
`make check` locally when changing build logic or symbols.

## PE.IMG Overlays

`tools/scripts/overlay_scan.py` can scan a user-supplied `PE.IMG` for code-like
overlay blocks:

```sh
.venv/bin/python tools/scripts/overlay_scan.py /path/to/PE.IMG --md
```

For the USA Disc 1 `PE.IMG` tested locally, the scanner reports 193 code blocks,
covering 1501 sectors, about 3002 KB. This is an estimate based on MIPS code
density and function prologues; no overlay bytes are committed to this repo.

## Copyright Policy

Do not commit copyrighted game data, extracted assets, generated split assembly,
memory cards, proprietary SDK/toolchain binaries, or files copied from Square
Enix/Sony releases.

Parasite Eve is owned by Square Enix. This project is an independent reverse
engineering/decompilation effort and is not affiliated with or endorsed by Square
Enix.
