# Inv_CompactActiveListSlots

Integrated in `src/main/item/Inv_CompactActiveListSlots.c`: all **340 linked
bytes** at `0x80054CF8` match retail. Stock native GCC 2.7.2 and stock MASPSX
use the existing `-G8` profile. No register pins, barriers, CPU instruction
assembly, or toolchain changes are needed. Two assembly-name aliases for the
base-capacity byte prevent its address from being kept across calls; these
are recorded in debt, following `Inv_CheckFreeSlotCapacity`.

For each pending slot, the routine finds the first empty slot within the
active list limit. If one exists, it computes capacity, moves the slot at
`capacity - pendingCount` into the hole, clears the source, and adjusts the
first matching tracked equipment index. Each iteration consumes one pending
slot even when no hole exists. This is not a general stable compaction pass:
a source equal to the destination ends up cleared, and equal tracked indices
only update the first entry. The shared `pe1/inventory_slots.h` supplies the
active-list state and prototype.

The capacity calculation intentionally preserves two helper calls below the
51-slot threshold, with a fresh capacity-byte read after each call. At or
above the threshold it uses 50. Nonpositive pending counts leave state alone.

Validation:

- `tools/tests/test_inventory_slot_pair.py` checks all linked bytes against
  a fixed retail SHA-256, independently of local game assets.
- `verify_behavior.py` passes 6,528 compaction cases against retail execution
  and a Python state model: empty and full lists, gaps and random layouts,
  several scan limits and pending counts, equipment index updates including
  equal indices, both capacity branches, and changing helper returns/base
  capacity. It verifies full slot storage, canaries, globals, helper traces,
  SP, GP and saved registers. Capacity helpers are modeled with emulator
  hooks; malformed pointers and out-of-range source indices are not covered.
- With the search helper, the oracle passes 8,976 cases. All 316 CI tests pass.
- The full EXE retains retail SHA-1
  `452fb033f2eaa4b18aa20a5bca60b8125af3a37b`.

Run from the repository root:

```sh
.venv/bin/python proposals/Inv_CompactActiveListSlots/verify_behavior.py \
  build/USA/src/main/item/Inv_FindFirstEmptySlot.c.o \
  build/USA/src/main/item/Inv_CompactActiveListSlots.c.o
```
