# parasite-eve-decomp

Work-in-progress matching decompilation of the North American PlayStation
release of Parasite Eve, target `SLUS-006.62`.

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

## Progress

See [docs/PROGRESS.md](docs/PROGRESS.md) for current per-binary progress
(regenerate with `make progress`). This README intentionally avoids hardcoded
function counts, percentages, or byte totals; the generated progress report is
the source of truth. A translation unit only counts as decompiled when it is
decompiled C rather than generated split assembly. Verified builds are
byte-identical to retail (`make check`, plus overlay checks for touched
overlays).

The matching policy is conservative: promoted decompilation work must match as
plain C through the stock compiler pipeline. Inline asm, register asm, asm
barriers, whole-function asm bodies, post-build rewrite passes, and build hacks
are not acceptable substitutes for decompilation.

## Layout

- `configs/USA/` - splat config, symbols, relocs, checksum.
- `src/main/` - decompiled C translation units for the main executable.
- `include/` - project headers.
- `tools/scripts/` - project-specific build and analysis helpers.
- `docs/` - progress table and maintained documentation.

Generated directories such as `asm/`, `linkers/`, `build/`, `assets/`, and
`disc/` are intentionally ignored, along with local working notes.

## Toolchain

Linking and assembling use standard GNU binutils: `mipsel-none-elf-as`,
`mipsel-none-elf-ld`, and `mipsel-none-elf-objcopy` on `PATH`.

Compiling matching C uses a native PSX GCC 2.7.2 `cc1` from
[decompals/old-gcc](https://github.com/decompals/old-gcc), plus a
PSYQ-compatible preprocessor and
[maspsx](https://github.com/mkst/maspsx) reproducing ASPSX assembler behavior.
The supported compiler path is the native toolchain below. Default local paths:

```text
sdk/psyq-4.0/PSX/INCLUDE/
tools/psyq-gcc-2.7.2/cpp
tools/old-gcc/cc1
tools/maspsx/maspsx.py
```

Overridable with environment variables:

```sh
export PE_CPP=/path/to/gcc-2.7.2/cpp
export PE_CC1=/path/to/native/old-gcc/cc1-or-wrapper
```

Install the local compiler pieces with:

```sh
tools/scripts/setup_stock_cc1.sh
tools/scripts/setup_maspsx.sh
```

Current matching note: `-G8` is part of the expected PSX small-data profile.
The high-count maspsx flags `--use-comm-section` and `--expand-div` were tested
for removal on 2026-07-07 and are still required: direct object probes produced
0 matches for both flag families, removing `--use-comm-section` breaks the final
link with discarded `.sbss` references, and removing `--expand-div` links but
fails the retail SHA-1 check. The delay-slot custom flags are likewise kept
until there is a faithful ASPSX-compatible replacement; this includes the
opt-in `%lo(symbol)` call/branch delay cases covered by `--la-call-delay`.

## Quick Start

```sh
python3 -m venv .venv
.venv/bin/pip install -r requirements.txt

mkdir -p assets/USA
cp /path/to/your/SLUS_006.62 assets/USA/main.exe

make split
make build
make check
```

`make split` regenerates `asm/` and `linkers/` from the user-supplied
executable, `make build` compiles and links `build/USA/main.exe`, and
`make check` verifies its SHA-1 against retail. Useful extras:

```sh
make func-diff FUNC=SomeFunction SRC=src/main/some_file.c
make permute FUNC=SomeFunction
make progress          # docs/PROGRESS.md + docs/progress.html + badges
make expected          # snapshot a byte-verified build for objdiff
make report            # objdiff-cli report (build/USA/report.json)
```

## License

See [LICENSE.md](LICENSE.md). Game code and data remain the property of their
respective owners; this repository contains only clean-room reimplementation
source.
