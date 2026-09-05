#!/usr/bin/env bash
# Download decompals/old-gcc stock gcc-2.7.2-psx cc1 into tools/old-gcc/cc1.
# The final build uses this release unmodified; local compiler patches are forbidden.
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
REL="https://github.com/decompals/old-gcc/releases/download/0.17"
case "$(uname -s)" in
  Darwin) ASSET="gcc-2.7.2-psx-macos.tar.gz" ;;
  Linux)  ASSET="gcc-2.7.2-psx.tar.gz" ;;
  *) echo "unsupported OS"; exit 1 ;;
esac
mkdir -p "$ROOT/tools/old-gcc"
tmp="$(mktemp -d "$ROOT/tools/old-gcc/.install.XXXXXX")"
trap 'rm -rf "$tmp"' EXIT
curl -fsSL -o "$tmp/cc1.tar.gz" "$REL/$ASSET"
tar xzf "$tmp/cc1.tar.gz" -C "$tmp"
cc1_bin="$(find "$tmp" -name cc1 -type f | head -1)"
chmod +x "$cc1_bin"
# The same release ships the matching preprocessor; cc.sh reads it from
# PE_CPP, and a cpp from any other gcc would predefine different macros.
cpp_bin="$(find "$tmp" -name cpp -type f | head -1)"
if [[ -n "$cpp_bin" ]]; then
    chmod +x "$cpp_bin"
    mv -f "$cpp_bin" "$ROOT/tools/old-gcc/cpp"
fi
# Publish only complete files; an executing compiler keeps its old inode.
mv -f "$cc1_bin" "$ROOT/tools/old-gcc/cc1"
echo "stock cc1 (and cpp) installed at tools/old-gcc/"
