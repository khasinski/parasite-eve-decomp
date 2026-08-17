#!/usr/bin/env bash
# Download decompals/old-gcc stock gcc-2.8.1-psx cc1 into tools/old-gcc-281/cc1.
# A handful of SDK/libgpu/libcd/libspu/cdrom/memcard functions were built by a
# 2.8.x SN toolchain that fills return/call delay slots; those TUs opt in per
# file with a `/* GCC_VERSION: 2.8.1 */` marker read by cc.sh. Everything else
# keeps stock gcc-2.7.2 (see setup_stock_cc1.sh).
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
REL="https://github.com/decompals/old-gcc/releases/download/0.17"
case "$(uname -s)" in
  Darwin) ASSET="gcc-2.8.1-psx-macos.tar.gz" ;;
  Linux)  ASSET="gcc-2.8.1-psx.tar.gz" ;;
  *) echo "unsupported OS"; exit 1 ;;
esac
mkdir -p "$ROOT/tools/old-gcc-281"
tmp="$(mktemp -d)"
curl -fsSL -o "$tmp/cc1.tar.gz" "$REL/$ASSET"
tar xzf "$tmp/cc1.tar.gz" -C "$tmp"
cp "$(find "$tmp" -name cc1 -type f | head -1)" "$ROOT/tools/old-gcc-281/cc1"
chmod +x "$ROOT/tools/old-gcc-281/cc1"
rm -rf "$tmp"
echo "stock cc1 (gcc-2.8.1) installed at tools/old-gcc-281/cc1"
