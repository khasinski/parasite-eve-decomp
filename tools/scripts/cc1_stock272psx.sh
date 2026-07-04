#!/usr/bin/env bash
# Default cc1: native old-gcc 2.7.2-psx (codegen-identical to CC1PSX Build 0001),
# falling back to Build 0001 via wibo on the rare inputs it segfaults on.
ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
STOCK="${PE_STOCK_CC1:-$ROOT/tools/old-gcc/cc1}"
if [[ -x "$STOCK" ]] && "$STOCK" "$@"; then
    exit 0
fi
exec "$ROOT/tools/scripts/cc1_build0001.sh" "$@"
