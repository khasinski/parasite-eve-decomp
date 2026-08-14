#!/usr/bin/env bash
# Download the stock decompals/old-gcc compilers this project builds with:
# gcc-2.7.2-psx as tools/old-gcc/cc1 (the game's own code) and
# gcc-2.8.1-psx as tools/old-gcc/cc1-2.8.1 (the vintage Sony built the
# PsyQ 4.x libraries with; TUs opt in via a CC1_VERSION comment).
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
REL="https://github.com/decompals/old-gcc/releases/download/0.17"
case "$(uname -s)" in
  Darwin) SUFFIX="-macos" ;;
  Linux)  SUFFIX="" ;;
  *) echo "unsupported OS"; exit 1 ;;
esac
mkdir -p "$ROOT/tools/old-gcc"
tmp="$(mktemp -d)"
curl -fsSL -o "$tmp/cc1.tar.gz" "$REL/gcc-2.7.2-psx$SUFFIX.tar.gz"
tar xzf "$tmp/cc1.tar.gz" -C "$tmp"
cp "$(find "$tmp" -name cc1 -type f | head -1)" "$ROOT/tools/old-gcc/cc1"
chmod +x "$ROOT/tools/old-gcc/cc1"
rm -rf "$tmp"; tmp="$(mktemp -d)"
curl -fsSL -o "$tmp/cc281.tar.gz" "$REL/gcc-2.8.1-psx$SUFFIX.tar.gz"
tar xzf "$tmp/cc281.tar.gz" -C "$tmp"
cp "$(find "$tmp" -name cc1 -type f | head -1)" "$ROOT/tools/old-gcc/cc1-2.8.1"
chmod +x "$ROOT/tools/old-gcc/cc1-2.8.1"
# The same release ships the matching preprocessor; cc.sh reads it from
# PE_CPP, and a cpp from any other gcc would predefine different macros.
cpp_bin="$(find "$tmp" -name cpp -type f | head -1)"
if [[ -n "$cpp_bin" ]]; then
    cp "$cpp_bin" "$ROOT/tools/old-gcc/cpp"
    chmod +x "$ROOT/tools/old-gcc/cpp"
fi
rm -rf "$tmp"
echo "stock cc1 (and cpp) installed at tools/old-gcc/"
