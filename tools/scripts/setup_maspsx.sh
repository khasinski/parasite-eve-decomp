#!/usr/bin/env bash
# Install the maspsx revision and PE1 compatibility patch used by this build.
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
MASPSX_DIR="$ROOT/tools/maspsx"
PATCH="$ROOT/tools/patches/maspsx-pe1.patch"
REV="42b862c988fe7a13fe4e7ac0ebec90ed6b9fb763"

if [[ ! -f "$PATCH" ]]; then
    echo "Error: missing patch at $PATCH" >&2
    exit 1
fi

if [[ ! -d "$MASPSX_DIR/.git" ]]; then
    mkdir -p "$(dirname "$MASPSX_DIR")"
    git clone https://github.com/mkst/maspsx.git "$MASPSX_DIR"
fi

if git -C "$MASPSX_DIR" apply --reverse --check "$PATCH"; then
    echo "maspsx PE1 patch already applied"
    exit 0
fi

if [[ -n "$(git -C "$MASPSX_DIR" status --porcelain)" ]]; then
    echo "Error: tools/maspsx has local changes and the PE1 patch is not applied cleanly" >&2
    echo "       Inspect tools/maspsx, then rerun this script." >&2
    exit 1
fi

git -C "$MASPSX_DIR" fetch --tags origin
git -C "$MASPSX_DIR" checkout "$REV"

if git -C "$MASPSX_DIR" apply --check "$PATCH"; then
    git -C "$MASPSX_DIR" apply "$PATCH"
else
    echo "Error: PE1 maspsx patch does not apply to $REV" >&2
    exit 1
fi

echo "maspsx installed at tools/maspsx"
