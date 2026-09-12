# Inv_FindFirstEmptySlot

Integrated in `src/main/item/Inv_FindFirstEmptySlot.c`: all **316 linked
bytes** at `0x8005382C` match retail. Stock native GCC 2.7.2 and stock MASPSX
use their existing `-G8` small-data profile. Two scan-limit register bindings
are recorded in debt; no barriers, CPU instruction assembly, or toolchain
changes are used.

The function selects Aya's inventory and selection-bit storage, obtains the
slot limit, then returns the first index of a contiguous run of empty slots.
It returns -1 when no run fits. The shared `pe1/inventory_slots.h` provides
all global declarations and the public prototype. The source expresses the
inner scan with explicit break conditions; GCC reproduces the retail loop
peeling without manually unrolling the run-length scan.

Retail behavior for nonpositive requested counts is retained: an initial
empty slot is still required, and the search can extend past the reported
capacity. The verification allocates enough backing slots for these cases;
it does not assert validity of arbitrary caller counts or pointers.

Validation:

- `tools/tests/test_inventory_slot_pair.py` checks the entire linked text
  against its fixed retail SHA-256 without requiring game assets in CI.
- `../Inv_CompactActiveListSlots/verify_behavior.py` passes 2,448 search cases
  against retail execution and an independent Python search. Cases include
  zero capacity, insufficient space, nonpositive counts, boundary and seeded
  random layouts, and a helper that redirects the active inventory pointer.
  It checks return values, selection globals, untouched slot storage and
  canaries, helper calls, SP, GP and all saved registers.
- The combined pair passes 8,976 semantic cases and all 316 CI tests.
- The complete EXE retains retail SHA-1
  `452fb033f2eaa4b18aa20a5bca60b8125af3a37b`.

See the companion README for the combined oracle command.
