#!/usr/bin/env bash
# Run splat on the SLUS_006.62 executable.
# Usage: tools/scripts/split.sh
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
cd "$ROOT"
exec "$ROOT/.venv/bin/python" -m splat split configs/USA/main.yaml "$@"
