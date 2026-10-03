# Render_SetupColorTable (0x800375E0, yaml `[0x27DE0, asm, main/task3_tail]`, 0x284 bytes)

Opens a free textbox entry in `g_TextboxEntries` (4 x 0x38): state=1,
page id, style, flags, cursor rect from D_8009CE98..9E when styled, and up to
five decimal arguments (terminated by -1) as least-significant-first digits
with a count byte at [5]. Needs `textbox_h.diff` (adds the `typed` byte at
+0x09, `args[5][6]` at +0x1A and the cursor/flag externs).

Status: not matched (stock GCC 2.7.2, -G8).

- `for_loop.c` (real `for` arg loop): inner `value == -1` constant is not
  hoisted (loop.c says "not desirable": savings 1, life 1 against 83 insns),
  and the `i*56 + base` invariant is hoisted as one sum, whereas retail hoists
  1, -1, the /10 magic and `&args` base to function entry and keeps `i*56`
  alone in the arg-loop preheader (address formed as (arg*6 + i*56) + base).
- `goto_loop.c` (goto arg loop): all constants hoist like retail, but `i*56`
  is recomputed in the loop body (no preheader), and goto adds crutch debt.
- Both: retail keeps the args pointer in t4 and page in t1 (a2 is reused for
  the i*56 array offset), style stays in a1 and is tested with `andi` at use.

Next idea: find the source shape that makes `-1` loop-invariant in the arg
loop (it is shared with `D_8009CEA4 = -1` in retail, register t6).
