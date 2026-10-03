# Menu_StepScrollCursor (0x80063E0C, 0x9C4 bytes, main.yaml 0x5460C)

Plain C, stock GCC 2.7.2, no pins, barriers or volatile. Compiles to 2496
bytes against retail's 2500: one instruction pair of block layout is left.

Everything else matches instruction for instruction once these were found:

- Retail inlines small helpers (cursor cell, target cell, scroll-to-cursor,
  shoulder-button query, swap with a linked list, restore a linked list's
  marked cell). Written as `static inline` functions they reproduce the
  unfolded `move v0,zero; andi v0,v0,0x20` and the per-call register reuse.
- `y_limit - ((cursor_x == 1 && has_scroll) + 1)` must come from an inline
  call (`MenuWidget_EndMargin`) or fold rewrites it to `(y_limit - 1) - e`;
  the page-down clamp is a MIN-style ternary that evaluates it twice.
- `limit - (wrap + 1)` in the other directions needs the `margin` temporary
  for the same reason.
- The up/down scroll steps read `scroll_y` in the comparison and copy it to
  `old` inside the block; their temporaries are block scoped so they land in
  a1/a0/v1 like retail.
- `disabled` (+0x40) is the half step stored into `scroll_adjust`.
- Left ends `if (flags & 1) changed |= held; else changed |= 1;`, right ends
  with the inverted test so their tails cross-jump like retail.

Remaining difference: jump2 cross-jumps the cancel case's second
`changed |= done` tail and the right-hand `changed |= held` into the left
case's identical `or s2,s2,v1; j` block. Retail keeps the left copy separate
and shares the cancel case's copy (L800646B8) with the right case instead,
so retail has one extra `j L800647B0; or s2,s2,v1` pair (4 instructions in
the left case, the cancel tail falls through). Statement order variants of
the left/right/cancel tails did not change the choice.

Header used while scoring: include/pe1/menu_scroll_cursor.h (inlined at the
top of the candidate).
