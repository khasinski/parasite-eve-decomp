#!/usr/bin/env bash
# Compile one C file with PSYQ GCC 2.7.2 -> maspsx -> mipsel-none-elf-as -> .o.
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/../.." && pwd)"

IN="$1"
OUT="$2"
shift 2

CC1="${PE_CC1:-$ROOT/tools/scripts/cc1_build0001.sh}"
MASPSX=("$ROOT/.venv/bin/python" "${PE_MASPSX:-$ROOT/tools/maspsx/maspsx.py}")
MASPSX_ASPSX_VERSION="${PE_MASPSX_ASPSX_VERSION:-2.56}"
AS=$(command -v mipsel-none-elf-as)
INCLUDE="${PE_PSYQ_INCLUDE:-$ROOT/sdk/psyq-4.0/PSX/INCLUDE}"

if [[ -n "${PE_CPP:-}" ]]; then
    CPP=("$PE_CPP")
elif [[ -x "$ROOT/tools/psyq-gcc-2.7.2/cpp" ]]; then
    CPP=("$ROOT/tools/psyq-gcc-2.7.2/cpp")
else
    WIBO="${PE_WIBO:-$ROOT/tools/wibo/wibo}"
    CPPPSX="${PE_CPPPSX_EXE:-$ROOT/sdk/psyq-4.0/COMPILER/CPPPSX.EXE}"
    CPP=("$WIBO" "$CPPPSX")
fi

CPP_FLAGS="-undef -D__GNUC__=2 -D__OPTIMIZE__ -Dmips -D__mips__ -D__LITTLE_ENDIAN__ -I$INCLUDE -I$ROOT/include $*"
CC1_FLAGS="-O2 -G0 -funsigned-char -mips1 -mcpu=3000"
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

for flag in -mdebuga -g3 -fno-schedule-insns -fno-schedule-insns2 \
            -fno-delayed-branch -fno-gcse -fno-cse-follow-jumps \
            -fno-cse-skip-blocks -fno-rerun-cse-after-loop -fno-thread-jumps; do
    if grep -q "CC1_FLAGS:.*$flag" "$IN"; then
        CC1_FLAGS="$CC1_FLAGS $flag"
    fi
done

mkdir -p "$(dirname "$OUT")"

TMP_I=$(mktemp -t pe1-cc.XXXXXX.i)
TMP_S=$(mktemp -t pe1-cc.XXXXXX.s)
trap 'rm -f "$TMP_I" "$TMP_S"' EXIT

"${CPP[@]}" $CPP_FLAGS "$IN" > "$TMP_I"
"$CC1" $CC1_FLAGS "$TMP_I" -o "$TMP_S"

MASPSX_EXTRA=()
for flag in --expand-div --no-div-use-nop --use-comm-section \
            --stack-return-delay --store-return-delay --load-dest-temp; do
    if grep -q "MASPSX_FLAGS:.*$flag" "$IN"; then
        MASPSX_EXTRA+=("$flag")
    fi
done
if [[ "$AS_G_FLAG" != "-G0" ]]; then
    MASPSX_EXTRA+=(--dont-force-G0)
fi

"${MASPSX[@]}" --aspsx-version="$MASPSX_ASPSX_VERSION" \
        ${MASPSX_EXTRA[@]+"${MASPSX_EXTRA[@]}"} \
        --gnu-as-path="$AS" \
        --run-assembler \
        -EL "$AS_G_FLAG" -march=r3000 -mtune=r3000 -no-pad-sections \
        -I"$ROOT/include" -I"$ROOT" \
        -o "$OUT" \
        < "$TMP_S"
