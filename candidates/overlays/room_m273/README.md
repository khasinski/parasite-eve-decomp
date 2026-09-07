# Directed particle rings: matching candidate

Continuation of the `.codex2` work last committed as `37c0682c`.
The interrupted experiment was `scratch/room273-directed-rings.c`.
This directory preserves the improved source and self-contained verification;
it does not depend on that scratch file or the previous session's `/tmp` tools.

`func_801969D8`, retail file offset `0x79F0`, VRAM `0x801969D8`, length
`0x554` (1364 bytes), now matches with stock `tools/scripts/cc.sh`.
The initial resumed object scored 2814/34100 in upstream asm-differ;
the final candidate scores 0/34100. More decisively, linking this object in
place of the configured assembly object preserves **every byte** of the
whole overlay, SHA-1 `692fdf1477e29d8eb1e2e6c8a7e1cc8ac51dda3a`.

## Behavior established from the retail instructions

- Mode 0 initializes a pool with 24-byte records, capacity 40, and callback
  `func_8019665C`, returning the initializer's result.
- Mode 1 returns 2 immediately when the stop byte at `0x8019AF69` is set.
  Otherwise it computes player-relative pitch using the square root and
  angle helpers, masks it to 12 bits, and clamps it to 384–768.
- When the emission flag at `0x8019AF68` is set, it attempts six particles
  followed by four. Each loop stops independently on allocation failure;
  failure in the first loop does not suppress the second loop or the final
  `0x5D1` event call. The pool pointer is loaded for every allocation.
- The first ring samples six angular positions with pitch amplitude 160;
  the second samples four with amplitude 96 and half the horizontal spread.
  Both apply the existing rotation/translation GTE operations to
  `D_8019AD60`, then store IR1/IR2/IR3 as the particle velocity in C.
  The origin is the three halfwords at `0x8019AEFC`; heading is at
  `0x8019AF60`. Position and velocity fourth halfwords are zeroed.
- Mode 2 configures the existing render globals and palette. Repeated
  width/height stores remain volatile, preserving the retail access sequence.
  Other modes return zero without invoking helpers.

Unsigned products express the retail 32-bit wraparound of squared distances.
The local names describe observed roles, not proven original type names.

## Reproduce

From the repository root, with the normal local compiler and retail assets:

```sh
make overlay-check OVERLAY=room_m273
python3 candidates/overlays/room_m273/verify_directed_rings.py
python3 candidates/overlays/room_m273/test_directed_rings.py
make verify
```

The verifier compiles the candidate and substitutes its object only in a
temporary copy of the generated linker script. Production objects, manifests,
baselines, and progress figures remain unchanged. Rebuild the overlay first
so its other objects and symbol scripts are current.

Validated 2026-09-07: normal overlay SHA, candidate-substituted overlay SHA,
and `make verify` (278 project tests and main executable SHA) all pass.
The separate host test exercises 315 combinations of angle and independent
allocation failure boundaries, plus stop/emission gates, initialization,
render configuration, inert modes, and distance overflow. Its GTE transfers
are mocks: it checks CPU inputs, output stores, and call sequencing, not
hardware GTE arithmetic. Host address aliases model the evidenced PSX window;
`-fno-strict-aliasing` is used only for this test, not the target build.

## Remaining constraints and promotion work

This is a **matched candidate**, not configured production C and not new
progress credit. Promotion would currently increase the independent debt
ratchets in `CONTRIBUTING.md` and `docs/CODE_ORGANIZATION.md`. Do not raise
those baselines just to integrate it, or move constraints into a header merely
to hide them from the direct-source counter.

After achieving an exact match, each pin and barrier was removed individually
and recompiled; two output-pointer pins were removable together. The retained
source has ten individual pinned variables (six declaration groups):

| Constraint removed | asm-differ score after removal |
| --- | ---: |
| Stop pointer, s1 | 25 |
| Heading pointer, s3 | 100 |
| Matrix pointer, s0 | 230 |
| Second ring table pointer, v1 | 420 |
| Each of six GTE result variables, t4/t5/t6 | 10 each |
| Each of two output-pointer barriers | 130 each |
| Second ring rotation-address input barrier | 30 |

These are local minimization results, not proof that a better C shape cannot
eliminate them. No ordinary CPU instruction ASM, new GTE macro, compiler
modification, or postpass was added.

Next work: remove remaining constraints or offset them with independently
verified debt reductions; centralize exact cross-unit declarations in a
suitable subsystem header; replace the provisional origin/heading byte window
with an evidenced shared record; and promote source plus manifest only with
the required clean verification. The current partial local record types and
extern declarations are candidate scaffolding, not a completed ABI audit.
