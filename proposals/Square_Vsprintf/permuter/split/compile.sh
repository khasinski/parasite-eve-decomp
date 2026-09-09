#!/bin/bash
set -euo pipefail
root=$(cd "$(dirname "$0")/.." && pwd)
intermediate=$(mktemp --suffix=.s)
trap 'rm -f "$intermediate"' EXIT
"$root/tools/cc1" -w -O2 -G0 -funsigned-char -mips1 -mcpu=3000 -fno-strength-reduce -fno-force-mem "$1" -o "$intermediate"
"$root/venv/bin/python" "$root/tools/maspsx/maspsx.py" --aspsx-version=2.56 --gnu-as-path=/usr/bin/mipsel-linux-gnu-as --run-assembler -EL -G0 -march=r3000 -mtune=r3000 -no-pad-sections -o "$3" < "$intermediate"
