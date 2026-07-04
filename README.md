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

See [docs/PROGRESS.md](docs/PROGRESS.md) for the per-binary table (regenerate
with `make progress`). A translation unit only counts as decompiled when it is
plain C with no assembly of any kind; for the main executable that is currently
59% of functions and 20% of code bytes. The built executable is byte-identical
to retail (`make check`).

The matching policy: every function must eventually match as plain C through
the stock compiler pipeline. No post-build rewrite passes, no inline assembly;
functions that cannot yet be matched that way stay as generated split assembly.

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

Compiling matching C uses the same compiler Square used: GCC 2.7.2.SN32.3.7
Build 0001 from PSY-Q 4.0 (`CPPPSX.EXE`/`CC1PSX.EXE`), run through
[wibo](https://github.com/decompals/wibo), with
[maspsx](https://github.com/mkst/maspsx) reproducing ASPSX assembler behavior.
Default local paths:

```text
sdk/psyq-4.0/COMPILER/CC1PSX.EXE
sdk/psyq-4.0/PSX/INCLUDE/
tools/psyq-gcc-2.7.2/cpp
tools/wibo/wibo
tools/maspsx/maspsx.py
```

Overridable with environment variables:

```sh
export PE_CPP=/path/to/gcc-2.7.2/cpp
export PE_CC1=/path/to/cc1-or-wrapper
export PE_CC1_BUILD0001_EXE=/path/to/CC1PSX.EXE
export PE_WIBO=/path/to/wibo
```

A native rebuild of cc1 2.7.2 (codegen-identical to Build 0001) can be
installed with `tools/scripts/setup_stock_cc1.sh`; the build falls back to
`CC1PSX.EXE` via wibo when it is absent.

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
make progress
```

## License

See [LICENSE.md](LICENSE.md). Game code and data remain the property of their
respective owners; this repository contains only clean-room reimplementation
source.
