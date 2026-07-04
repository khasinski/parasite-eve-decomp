#!/usr/bin/env bash
# Wrapper around the original PSYQ 4.0 CC1PSX.EXE Build 0001 via decompals/wibo.
# Drop-in replacement for cc1: accepts same flags, reads preprocessed input
# file, writes assembly output.
#
# Usage: cc1_build0001.sh [flags...] <input.i> -o <output.s>
#
# The original CC1PSX.EXE is the actual compiler used to build SLUS_006.62.
# Build 0004 (community recompile in tools/psyq-gcc-2.7.2/) is PE2's vintage
# and produces different codegen for some functions.
#
# Uses wibo (https://github.com/decompals/wibo) — a thin Win32 PE loader.
# Much faster than wine + docker (no startup overhead per call).
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
CC1_EXE="${PE_CC1_BUILD0001_EXE:-$ROOT/sdk/psyq-4.0/COMPILER/CC1PSX.EXE}"
WIBO="${PE_WIBO:-$ROOT/tools/wibo/wibo}"

if [[ ! -x "$WIBO" ]]; then
    echo "Error: wibo not found at $WIBO" >&2
    echo "  Install with: mkdir -p tools/wibo && curl -fL -o tools/wibo/wibo \\" >&2
    echo "    https://github.com/decompals/wibo/releases/download/1.1.0/wibo-macos && chmod +x tools/wibo/wibo" >&2
    exit 1
fi

exec "$WIBO" "$CC1_EXE" "$@"
