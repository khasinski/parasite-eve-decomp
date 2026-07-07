#!/usr/bin/env bash
# Default cc1: native old-gcc 2.7.2-psx.
ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
STOCK="${PE_STOCK_CC1:-$ROOT/tools/old-gcc/cc1}"
if [[ ! -x "$STOCK" ]]; then
    echo "Error: native old-gcc cc1 not found at $STOCK" >&2
    echo "  Install with: tools/scripts/setup_stock_cc1.sh" >&2
    exit 1
fi
exec "$STOCK" "$@"
