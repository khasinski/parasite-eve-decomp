#!/usr/bin/env bash
# Build unmodified GNU binutils 2.8.1 GAS for its historical MIPS division macros.
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
DEST="${PE_GAS281_DIR:-$ROOT/tools/gas-2.8.1}"
# Rebuild binaries installed before the host optimization fix.
BUILD_CONFIG=host-o0-v1
if [ -x "$DEST/as" ] && [ "$(cat "$DEST/build-config" 2>/dev/null || true)" = "$BUILD_CONFIG" ]; then
    exit 0
fi
HOST_CC="${PE_HOST_CC:-cc}"
# Modern host GCC at -O2 can miscompile this vintage assembler: Linux builds
# duplicate the lw opcode over its preceding lui during macro relaxation.
# Keep host optimization disabled; the linked-byte regression covers both hosts.
FLAGS='-O0 -fcommon -std=gnu89 -D_POSIX_C_SOURCE=200809L'
if [ "$(uname -s)" = Darwin ]; then
    if [ -z "${PE_HOST_CC:-}" ]; then
        HOST_CC=''
        for version in 16 15 14 13; do
            if command -v "gcc-$version" >/dev/null; then HOST_CC="gcc-$version"; break; fi
        done
        [ -n "$HOST_CC" ] || { echo 'Install native GCC with brew install gcc.' >&2; exit 1; }
    fi
    FLAGS="$FLAGS -DNSIG=__DARWIN_NSIG"
fi
if [ "$(uname -s)" = Linux ]; then FLAGS="$FLAGS -DNSIG=_NSIG"; fi
mkdir -p "$DEST"
WORK="$(mktemp -d "$DEST/.install.XXXXXX")"
trap 'rm -rf "$WORK"' EXIT
curl -fsSL https://ftp.gnu.org/gnu/binutils/binutils-2.8.1.tar.gz -o "$WORK/source.tar.gz"
printf '%s  %s\n' c7ad94c18f098f0580d0a7cd68638e98aa98571078536934fdc85f5e9047bf5a "$WORK/source.tar.gz" | shasum -a 256 -c -
tar xzf "$WORK/source.tar.gz" -C "$WORK"
mkdir "$WORK/build"
(
    cd "$WORK/build"
    # The 1996 config.sub predates current host names. This compatibility name
    # selects generic Unix support; HOST_CC still builds a native executable.
    # Explicitly describe the real host long type to BFD on 64-bit hosts.
    if [ "$(getconf LONG_BIT)" = 64 ]; then export HOST_64BIT_TYPE=long; fi
    CC="$HOST_CC" CFLAGS="$FLAGS" ../binutils-2.8.1/configure --host=i386-pc-linux \
        --target=mipsel-unknown-elf --disable-nls >configure.log 2>&1 || { cat configure.log >&2; exit 1; }
    make -j"${PE_BUILD_JOBS:-4}" CFLAGS="$FLAGS" all-gas >build.log 2>&1 || { tail -100 build.log >&2; exit 1; }
)
chmod +x "$WORK/build/gas/as.new"
mv -f "$WORK/build/gas/as.new" "$DEST/as"
printf '%s\n' "$BUILD_CONFIG" >"$DEST/build-config"
echo "Native GNU GAS 2.8.1 installed at $DEST/as"
