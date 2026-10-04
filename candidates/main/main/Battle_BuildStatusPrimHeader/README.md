# Battle_BuildStatusPrimHeader (main 0x21F60, 1548 bytes): lev 49

Typed rewrite of the old byte-offset draft (that draft scored lev 309). Plain C,
no pins, barriers, volatile, asm aliases or byte-pointer arithmetic. The
packets are typed through `include/pe1/battle_status.h`:
`BattleStatusPointerPrim D_8009E4D8[2]` (flat triangle), `RenderLinePacket
D_8009E498[2][2]` (two connector lines per draw slot), the existing
`BattleGaugePrim D_8009E460[2]` sprite and `BattleStatusLinePrim D_8009E358`
panel origin.

Score: `lev.py <obj> main 21F60 60C --func Battle_BuildStatusPrimHeader` gives
lev 49 (retail 387 words, mine 378).

## What is already right

- Rotation/translation set-up: the `{0, 0, angle}` and `{x, y, 0}`
  initializers reproduce retail's memset temp plus copy, because safe_from_p
  rejects building a constructor straight into a stack variable when it
  reads a global.
- The x and y connector blocks match exactly (registers included). Keys:
  `s16 x`/`s16 y` (lh plus `move` copy), `x -= 8` in place on the right side,
  block-local `lineX`/`panelX` on the left, a block-local `RenderLinePacket
  *lines = D_8009E498[slot]` for the [1] line (register base, offsets 24/28)
  while the [0] line stays indexed (absolute `%lo` form), and one load of
  `projected_target_x` shared by the -35 and -115 values.
- Sprite g/b stores recompute the packet address in a fresh register (retail
  re-reads the draw slot after each `sb`): a `static inline` helper returning
  `&D_8009E460[g_ActiveDrawSlot].sprite` does it.

## Remaining diffs (49 words)

1. Frame size, 8 words: retail has a 0x120 frame and this draft has 0x100.
   The extra 32 bytes are four more dead 8-byte slots. They come from
   `(use (reg))` insns that combine puts at labels for orphaned death notes
   (this draft has 4, at the color, x and y joins). An unused 32-byte local
   would close the gap, but that is an invented local and is not used here.
2. Color block, about 17 words: retail keeps the tri.g store in both branches
   and cross-jumps only the final `tri.b` address and store. This draft stores
   `tri.b` after the join from `shade`, so jump2 merges the tri.g store as well.
   With the stores fully duplicated (no `shade`), jump2 merges everything up to
   the `li` (lev +16). I could not find out what stops retail's merge at the
   tri.b slot load.
3. Prologue registers, about 19 words: the `offset` block copy uses v1..a2
   instead of v0..a1, and the first slot/`*28`/sprite registers are rotated
   (v0/v1/v1 instead of v1/v0/a0). sched1 puts the slot load above the BLKmode
   copy (it fills the load-latency gap), so the slot pseudo is live across the
   movstr and takes v0. In retail the load is below the copy.
4. Triangle base, 5 words: `la` for D_8009E4D8 is in v0 instead of v1 (it is a
   tie-break in sched1's ready list between the la, the slot*20 shift and the
   projected[0].x load).

A permuter run on darwine (bpq7) found nothing better that keeps the C
semantics.
