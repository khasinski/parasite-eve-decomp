#!/usr/bin/env bash
# Download the objdiff-cli release this project pins. The same binary that
# generates the decomp.dev report in CI generates it locally, so the numbers
# can never disagree because of a tool version.
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/../.." && pwd)"

VERSION="v3.8.0"
case "$(uname -s)-$(uname -m)" in
  Darwin-arm64)  ASSET="objdiff-cli-macos-arm64" ;;
  Darwin-x86_64) ASSET="objdiff-cli-macos-x86_64" ;;
  Linux-x86_64)  ASSET="objdiff-cli-linux-x86_64" ;;
  Linux-aarch64) ASSET="objdiff-cli-linux-aarch64" ;;
  *) echo "unsupported platform: $(uname -s)-$(uname -m)" >&2; exit 1 ;;
esac

mkdir -p "$ROOT/tools/objdiff"
curl -fsSL -o "$ROOT/tools/objdiff/objdiff-cli" \
    "https://github.com/encounter/objdiff/releases/download/$VERSION/$ASSET"
chmod +x "$ROOT/tools/objdiff/objdiff-cli"
echo "objdiff-cli $VERSION installed at tools/objdiff/objdiff-cli"
