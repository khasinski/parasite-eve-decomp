#!/usr/bin/env bash
# Parallel byte-check of room overlays.
#   room_sweep.sh            - all rooms
#   room_sweep.sh m014 m065  - only listed rooms (accepts room_mNNN too)
set -uo pipefail
cd "$(dirname "$0")/../.."
if [ $# -gt 0 ]; then
  rooms=$(for r in "$@"; do echo "${r#room_}" | sed 's/^/room_/'; done)
else
  rooms=$(ls configs/USA/overlays/room_*.yaml | xargs -n1 basename | sed 's/\.yaml//')
fi
echo "$rooms" | xargs -P 8 -I{} sh -c \
  'make overlay-check OVERLAY={} >/dev/null 2>&1 || echo "FAIL {}"'
echo "swept $(echo "$rooms" | wc -w | tr -d " ") rooms"
