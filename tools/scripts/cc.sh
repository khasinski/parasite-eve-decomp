#!/usr/bin/env bash
# Compile one C file with native PSX GCC 2.7.2 → maspsx → mipsel-none-elf-as → .o.
# Usage: cc.sh <input.c> <output.o> [extra cflags...]
#
# Pipeline: PSYQ-compatible cpp handles -I/-D, then native old-gcc PSX cc1
# compiles preprocessed C, then maspsx wraps the asm output, then GNU as
# produces the .o.
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/../.." && pwd)"

IN="$1"
OUT="$2"
shift 2

CPP="${PE_CPP:-$ROOT/tools/psyq-gcc-2.7.2/cpp}"
# Default cc1: native old-gcc 2.7.2-psx; install it with
# tools/scripts/setup_stock_cc1.sh if missing.
CC1="${PE_CC1:-$ROOT/tools/scripts/cc1_stock272psx.sh}"
MASPSX=("$ROOT/.venv/bin/python" "$ROOT/tools/maspsx/maspsx.py")
MASPSX_ASPSX_VERSION="${PE_MASPSX_ASPSX_VERSION:-2.56}"
AS=$(command -v mipsel-none-elf-as)
INCLUDE="$ROOT/sdk/psyq-4.0/PSX/INCLUDE"

CPP_FLAGS="-undef -D__GNUC__=2 -D__OPTIMIZE__ -Dmips -D__mips__ -D__LITTLE_ENDIAN__ -I$INCLUDE -I$ROOT/include $*"
CC1_FLAGS="-w -O2 -G0 -funsigned-char -mips1 -mcpu=3000"
AS_G_FLAG="-G0"
if grep -q 'CC1_FLAGS:.*-G4' "$IN"; then
    CC1_FLAGS="${CC1_FLAGS/-G0/-G4}"
elif grep -q 'CC1_FLAGS:.*-G8' "$IN"; then
    CC1_FLAGS="${CC1_FLAGS/-G0/-G8}"
fi
if grep -q 'MASPSX_FLAGS:.*-G4' "$IN"; then
    AS_G_FLAG="-G4"
elif grep -q 'MASPSX_FLAGS:.*-G8' "$IN"; then
    AS_G_FLAG="-G8"
fi
if grep -q 'CC1_FLAGS:.*-O1' "$IN"; then
    CC1_FLAGS="${CC1_FLAGS/-O2/-O1}"
elif grep -q 'CC1_FLAGS:.*-O3' "$IN"; then
    CC1_FLAGS="${CC1_FLAGS/-O2/-O3}"
fi
if grep -q 'CC1_FLAGS:.*-mdebuga' "$IN"; then
    CC1_FLAGS="$CC1_FLAGS -mdebuga"
fi
if grep -q 'CC1_FLAGS:.*-g3' "$IN"; then
    CC1_FLAGS="$CC1_FLAGS -g3"
fi
if grep -qE 'CC1_FLAGS:.*-fno-schedule-insns([^2]|$)' "$IN"; then
    CC1_FLAGS="$CC1_FLAGS -fno-schedule-insns"
fi
if grep -q 'CC1_FLAGS:.*-fno-schedule-insns2' "$IN"; then
    CC1_FLAGS="$CC1_FLAGS -fno-schedule-insns2"
fi
if grep -q 'CC1_FLAGS:.*-fno-delayed-branch' "$IN"; then
    CC1_FLAGS="$CC1_FLAGS -fno-delayed-branch"
fi
if grep -q 'CC1_FLAGS:.*-fno-gcse' "$IN"; then
    CC1_FLAGS="$CC1_FLAGS -fno-gcse"
fi
if grep -q 'CC1_FLAGS:.*-fno-cse-follow-jumps' "$IN"; then
    CC1_FLAGS="$CC1_FLAGS -fno-cse-follow-jumps"
fi
if grep -q 'CC1_FLAGS:.*-fno-cse-skip-blocks' "$IN"; then
    CC1_FLAGS="$CC1_FLAGS -fno-cse-skip-blocks"
fi
if grep -q 'CC1_FLAGS:.*-fno-rerun-cse-after-loop' "$IN"; then
    CC1_FLAGS="$CC1_FLAGS -fno-rerun-cse-after-loop"
fi
if grep -q 'CC1_FLAGS:.*-fno-thread-jumps' "$IN"; then
    CC1_FLAGS="$CC1_FLAGS -fno-thread-jumps"
fi

mkdir -p "$(dirname "$OUT")"

TMP_I=$(mktemp -t pe1-cc.XXXXXX.i)
TMP_S=$(mktemp -t pe1-cc.XXXXXX.s)
trap 'rm -f "$TMP_I" "$TMP_S"' EXIT

"$CPP" $CPP_FLAGS "$IN" -o "$TMP_I"
"$CC1" $CC1_FLAGS "$TMP_I" -o "$TMP_S"

MASPSX_EXTRA=()
if grep -q 'MASPSX_FLAGS:.*--expand-div' "$IN"; then
    MASPSX_EXTRA+=(--expand-div)
fi
if grep -q 'MASPSX_FLAGS:.*--use-comm-section' "$IN"; then
    MASPSX_EXTRA+=(--use-comm-section)
fi
if grep -q 'MASPSX_FLAGS:.*--stack-return-delay' "$IN"; then
    MASPSX_EXTRA+=(--stack-return-delay)
fi
if grep -q 'MASPSX_FLAGS:.*--store-return-delay' "$IN"; then
    MASPSX_EXTRA+=(--store-return-delay)
fi
if grep -q 'MASPSX_FLAGS:.*--la-return-delay' "$IN"; then
    MASPSX_EXTRA+=(--la-return-delay)
fi
if grep -q 'MASPSX_FLAGS:.*--la-call-delay' "$IN"; then
    MASPSX_EXTRA+=(--la-call-delay)
fi
if grep -q 'MASPSX_FLAGS:.*--store-call-delay' "$IN"; then
    MASPSX_EXTRA+=(--store-call-delay)
fi
if grep -q 'MASPSX_FLAGS:.*--store-jump-delay' "$IN"; then
    MASPSX_EXTRA+=(--store-jump-delay)
fi
if grep -q 'MASPSX_FLAGS:.*--store-branch-delay' "$IN"; then
    MASPSX_EXTRA+=(--store-branch-delay)
fi
if grep -q 'MASPSX_FLAGS:.*--load-dest-temp' "$IN"; then
    MASPSX_EXTRA+=(--load-dest-temp)
fi
if [[ "$AS_G_FLAG" != "-G0" ]]; then
    MASPSX_EXTRA+=(--dont-force-G0)
fi

"${MASPSX[@]}" --aspsx-version="$MASPSX_ASPSX_VERSION" \
        ${MASPSX_EXTRA[@]+"${MASPSX_EXTRA[@]}"} \
        --gnu-as-path="$AS" \
        --run-assembler \
        -EL "$AS_G_FLAG" -march=r3000 -mtune=r3000 -no-pad-sections \
        -I "$ROOT" -I "$ROOT/include" -I "$ROOT/asm/USA/main" -I "$ROOT/asm/USA/overlays" \
        -o "$OUT" \
        < "$TMP_S"
