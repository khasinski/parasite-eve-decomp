#!/usr/bin/env bash
# Build unmodified GNU binutils 2.7 GAS for its historical MIPS division macros.
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
DEST="${PE_GAS27_DIR:-$ROOT/tools/gas-2.7}"
[ ! -x "$DEST/as" ] || exit 0
HOST_CC="${PE_HOST_CC:-cc}"
FLAGS='-O2 -fcommon -std=gnu89 -D_POSIX_C_SOURCE=200809L'
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
curl -fsSL https://ftp.gnu.org/gnu/binutils/binutils-2.7.tar.gz -o "$WORK/source.tar.gz"
printf '%s  %s\n' 3f336a30166b85fa894271b4663a0ec427d797019cda8c468e7ba1b13baa162b "$WORK/source.tar.gz" | shasum -a 256 -c -
tar xzf "$WORK/source.tar.gz" -C "$WORK"
mkdir "$WORK/build"
(
    cd "$WORK/build"
    # The 1996 config.sub predates current host names. This compatibility name
    # selects generic Unix support; HOST_CC still builds a native executable.
    # Explicitly describe the real host long type to BFD on 64-bit hosts.
    if [ "$(getconf LONG_BIT)" = 64 ]; then export HOST_64BIT_TYPE=long; fi
    CC="$HOST_CC" ../binutils-2.7/configure --host=i386-pc-linux \
        --target=mipsel-unknown-elf --disable-nls >configure.log 2>&1 || { cat configure.log >&2; exit 1; }
    make -j"${PE_BUILD_JOBS:-4}" CFLAGS="$FLAGS" all-gas >build.log 2>&1 || { tail -100 build.log >&2; exit 1; }
)
chmod +x "$WORK/build/gas/as.new"
mv -f "$WORK/build/gas/as.new" "$DEST/as"
echo "Native GNU GAS 2.7 installed at $DEST/as"
