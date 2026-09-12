#!/usr/bin/env bash
# Build an unmodified bare-metal MIPS linker, including historical ELF support.
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
DEST="${PE_LINKER_DIR:-$ROOT/tools/binutils-2.45}"
[ ! -x "$DEST/bin/mipsel-none-elf-ld" ] || exit 0
mkdir -p "$DEST"
WORK="$(mktemp -d "$DEST/.install.XXXXXX")"
trap 'rm -rf "$WORK"' EXIT
curl -fsSL https://ftp.gnu.org/gnu/binutils/binutils-2.45.tar.xz -o "$WORK/source.tar.xz"
printf '%s  %s\n' c50c0e7f9cb188980e2cc97e4537626b1672441815587f1eab69d2a1bfbef5d2 "$WORK/source.tar.xz" | shasum -a 256 -c -
tar xf "$WORK/source.tar.xz" -C "$WORK"
mkdir "$WORK/build"
(
    cd "$WORK/build"
    ../binutils-2.45/configure --target=mipsel-none-elf --disable-nls \
        --disable-werror --disable-gdb --disable-gprofng --without-debuginfod \
        >configure.log 2>&1 || { cat configure.log >&2; exit 1; }
    make -j"${PE_BUILD_JOBS:-4}" MAKEINFO=true all-ld >build.log 2>&1 || { tail -100 build.log >&2; exit 1; }
)
mkdir -p "$DEST/bin"
mv "$WORK/build/ld/ld-new" "$DEST/bin/mipsel-none-elf-ld"
echo "Native GNU linker installed at $DEST/bin/mipsel-none-elf-ld"
