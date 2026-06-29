#!/usr/bin/env bash
# Run the original PSYQ CC1PSX.EXE through wibo.
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
CC1_EXE="${PE_CC1_BUILD0001_EXE:-$ROOT/sdk/psyq-4.0/COMPILER/CC1PSX.EXE}"
WIBO="${PE_WIBO:-$ROOT/tools/wibo/wibo}"

if [[ ! -x "$WIBO" ]]; then
    echo "Error: wibo not found at $WIBO" >&2
    exit 1
fi

exec "$WIBO" "$CC1_EXE" "$@"
