#!/usr/bin/env bash
# Install the stock maspsx revision used by this build.
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
MASPSX_DIR="$ROOT/tools/maspsx"
REV="42b862c988fe7a13fe4e7ac0ebec90ed6b9fb763"

if [[ ! -d "$MASPSX_DIR/.git" ]]; then
    mkdir -p "$(dirname "$MASPSX_DIR")"
    git clone https://github.com/mkst/maspsx.git "$MASPSX_DIR"
fi

if [[ -n "$(git -C "$MASPSX_DIR" status --porcelain)" ]]; then
    echo "Error: tools/maspsx has local changes; stock maspsx is required" >&2
    echo "       Remove or restore that checkout, then rerun this script." >&2
    exit 1
fi

git -C "$MASPSX_DIR" fetch --tags origin
git -C "$MASPSX_DIR" checkout "$REV"

echo "stock maspsx installed at tools/maspsx"
