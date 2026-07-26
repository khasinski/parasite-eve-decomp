# Room overlay split audit

Generated from the current room YAMLs and generated asm. Re-run the
consistency checks with `make room-split-audit`.

This document tracks why room overlays can show high function coverage but low
code-byte coverage, and which remaining areas are actual split problems versus
large non-decompiled functions.

## Current byte shape

The room-overlay progress row is:

| Area | Value |
|---|---:|
| Functions | 6115/6454 (94.7%) |
| Code bytes | 471620/2392460 (19.7%) |

The low byte percentage is not caused by room assets being included in the code
denominator. `tools/scripts/progress_report.py` counts function subsegments for
code bytes and excludes baked data carriers.

Current room YAML subsegment bytes:

| YAML type | Bytes |
|---|---:|
| `data` | 30322572 |
| `asm` | 1920840 |
| `c` | 471620 |
| `rodata` | 59112 |

So the missing code-byte coverage is primarily the remaining `asm` function
subsegments. The huge room data blocks are real room payloads/assets and are not
the reason code-byte coverage is low.

## Split checks already performed

These checks were run against current YAMLs and generated asm:

| Check | Result |
|---|---:|
| Room asm subsegments with exact generated file | 2254 |
| Missing exact asm files | 0 |
| Ambiguous/multi-function exact asm files | 0 |
| `nonmatching` size mismatches vs YAML range | 0 |
| `// type:func` symbols located inside YAML `data` ranges | 0 |
| Remaining base `jtbl_*` labels in room rodata | 0 |
| Safe historical asm subsegment rename candidates | 0 |
| Internal aligned `data` labels still hidden in larger `data` blocks | 0 |

This means the current room function split is internally consistent: the
remaining low percentage is not explained by known functions hidden inside data
or by asm ranges with wrong lengths.

The room YAMLs also no longer use the old generated `room_m...` names for
single-function asm subsegments when the exact generated file exposes one
unambiguous `glabel`; those subsegments now use the actual function labels.

The room data split is also less anonymous now. A previous audit found 2144
aligned internal `dlabel` boundaries inside larger room `data` subsegments; the
YAMLs now split those boundaries explicitly. This separates room control tables,
small referenced records, and large asset banks without moving any bytes into
the code denominator.

One caveat: old generated files under `asm/USA/overlays/<room>/` can remain
after split changes and may contain stale multi-function blobs. Do not use a
directory-wide scan of `asm/` as authoritative unless it checks the exact file
named by the current YAML subsegment.

## Largest remaining asm families

The largest repeated room functions by total remaining asm bytes are:

| Function family | Count | Total bytes |
|---|---:|---:|
| `RoomLib_HandlerE` | 109 | 154780 |
| `RoomLib_HandlerD` | 111 | 148316 |
| `RoomLib_HandlerC` | 109 | 125132 |
| `RoomLib_HandlerB` | 109 | 98972 |
| `RoomLib_DlgBlob` | 13 | 32448 |
| `func_80192F00` | 8 | 28384 |
| `func_80191100` large handler variant | 29 | 26912 |
| `func_80191D08` large handler variant | 29 | 26216 |
| `func_80192700` | 9 | 23688 |
| `func_8018F374` | 5 | 20740 |

The best byte-return work is therefore not more data splitting; it is
decompiling the shared room handler families, especially `RoomLib_HandlerE/D/C/B`.

## Important pitfalls

### Local names are not stable semantic identifiers

The same local symbol name can refer to very different bodies in different room
families. For example, `func_80191100` is an empty 8-byte callback in some rooms
but a large movement handler in others. Group candidates by body shape, size,
and YAML range, not just by symbol name.

### Some parsers fall through to the next return-zero function

Several argument parsers have no local epilogue. Their cases branch to the next
8-byte `return 0` function, for example the `room_m014` parser at `0x29C`.
Plain C naturally emits its own `jr $ra`, so matching these requires a careful
segment/source strategy that keeps the shared return target intact.

Do not convert these blindly; they can look easy but fail because of the shared
epilogue.

### GTE/scratchpad handlers are structurally hard

`RoomLib_HandlerB` and related movement handlers use scratchpad addresses around
`0x1F800000` and GTE instructions (`ctc2`, `lwc2`, `mvmva`, `swc2`). Those are
real code, not data. They need the surrounding structures and a pure-C/GTE
intrinsic strategy before they are good candidates for a wide mechanical pass.

## Next recommended work

1. Decompile one representative `RoomLib_HandlerB` variant only after modeling
   the scratchpad/GTE block in `room_lib.h`.
2. Use that structure to attack `RoomLib_HandlerC/D/E`; they account for the
   largest repeated byte mass.
3. For non-GTE work, prefer repeated medium functions with normal epilogues
   before parser functions that tail-jump into neighboring return-zero stubs.
4. Keep using the split audit checks before changing YAML: exact asm file,
   one `nonmatching`, size equals YAML range, and no `type:func` symbols inside
   data.
