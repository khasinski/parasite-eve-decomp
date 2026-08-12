#!/usr/bin/env bash
# Download the stock decompals/old-gcc PSX compiler and preprocessor.
# This is a source-buildable, patchable compiler.
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
REL="https://github.com/decompals/old-gcc/releases/download/0.17"
case "$(uname -s)" in
  Darwin) ASSET="gcc-2.7.2-psx-macos.tar.gz" ;;
  Linux)  ASSET="gcc-2.7.2-psx.tar.gz" ;;
  *) echo "unsupported OS"; exit 1 ;;
esac
mkdir -p "$ROOT/tools/old-gcc"
tmp="$(mktemp -d)"
curl -fsSL -o "$tmp/cc1.tar.gz" "$REL/$ASSET"
tar xzf "$tmp/cc1.tar.gz" -C "$tmp"
cp "$(find "$tmp" -name cc1 -type f | head -1)" "$ROOT/tools/old-gcc/cc1"
cp "$(find "$tmp" -name cpp -type f | head -1)" "$ROOT/tools/old-gcc/cpp"
chmod +x "$ROOT/tools/old-gcc/cc1" "$ROOT/tools/old-gcc/cpp"
rm -rf "$tmp"
echo "stock cc1 and cpp installed at tools/old-gcc"
