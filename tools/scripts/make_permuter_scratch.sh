#!/usr/bin/env bash
# Create a decomp-permuter scratch for one function, wired to our toolchain.
#   make_permuter_scratch.sh <src.c> [func]
# - base.c   : preprocessed source (pycparser-parseable), with the file's
#              CC1_FLAGS/MASPSX_FLAGS comment lines preserved for cc.sh.
# - target.o : the current ROM-correct build object.
# - compile.sh: compiles a candidate via cc.sh.
# - settings.toml: gcc + func_name.
# The permuter searches C variants whose codegen matches the ROM.
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
SRC="$1"
[[ -f "$SRC" ]] || { echo "no such source: $SRC"; exit 1; }
rel="${SRC#"$ROOT/"}"; rel="${rel#./}"   # path relative to repo root
OBJ="$ROOT/build/USA/${rel}.o"
[[ -f "$OBJ" ]] || { echo "no build object (build the repo first): $OBJ"; exit 1; }
FUNC="${2:-}"
if [[ -z "$FUNC" ]]; then
  FUNC="$(mipsel-none-elf-nm "$OBJ" | awk '$2=="T"{print $3; exit}')"
fi
[[ -n "$FUNC" ]] || { echo "could not detect function"; exit 1; }

SCR="$ROOT/tools/decomp-permuter/scratch_${FUNC}"
mkdir -p "$SCR"
cp "$OBJ" "$SCR/target.o"
# preserve toolchain flag comment lines (cpp would strip them)
grep -E '/\* (CC1_FLAGS|MASPSX_FLAGS):' "$SRC" > "$SCR/base.c" || true
"$ROOT/tools/psyq-gcc-2.7.2/cpp" -undef -P -D__GNUC__=2 -D__OPTIMIZE__ -Dmips \
  -D__mips__ -D__LITTLE_ENDIAN__ -D'__attribute__(x)=' \
  -I"$ROOT/sdk/psyq-4.0/PSX/INCLUDE" -I"$ROOT/include" "$SRC" >> "$SCR/base.c" 2>/dev/null
cat > "$SCR/compile.sh" <<EOF
#!/usr/bin/env bash
exec "$ROOT/tools/scripts/cc.sh" "\$1" "\$3"
EOF
chmod +x "$SCR/compile.sh"
printf 'compiler_type = "gcc"\nfunc_name = "%s"\n' "$FUNC" > "$SCR/settings.toml"
# sanity compile
if "$SCR/compile.sh" "$SCR/base.c" -o /tmp/_perm_sanity.o >/dev/null 2>&1; then
  echo "scratch ready: $SCR  (func=$FUNC)"
else
  echo "WARNING: base.c does not compile (m2c macros / parse issue): $SCR"
fi
