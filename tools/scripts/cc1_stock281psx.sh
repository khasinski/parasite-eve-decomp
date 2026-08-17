#!/usr/bin/env bash
# Stock gcc-2.8.1 PSX cc1, selected per file by cc.sh when a translation unit
# carries a `/* GCC_VERSION: 2.8.1 */` marker. Downloaded by setup_stock_cc281.sh.
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
CC1="$ROOT/tools/old-gcc-281/cc1"
[ -x "$CC1" ] || "$ROOT/tools/scripts/setup_stock_cc281.sh" >&2
exec "$CC1" "$@"
