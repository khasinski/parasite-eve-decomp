# menu_memcard func_80122040 (0x1340) / func_80192934 (0xA144): video step

Per-frame step of the video player: page flip request, decode/upload start,
2000-poll wait for a decoded frame, stream restart on a stall, volatile
0x800000 wait for the upload, finish check. Both copies identical apart
from relocations; one template `Memcard_StepVideo.inc`, instance
`Memcard_StepVideo_80122040.c` (the 0xA144 copy uses the template defaults,
name Memcard_StepVideo, which Memcard_PlayVideo calls as func_80192934).
Apply `menu_memcard_video.h.patch` (shared with the OpenVideo candidate).

Status (2026-10-04, agent 5): parked, about 200 aligned diff lines left
(size 0x314 retail vs 0x304), frame and register set already right
(0x30, s0..s4).

Solved, keep these:
- `if (D_800B0DBA >= 2) { ... } return 0;` gives retail's `bnez -> shared
  move v0,zero` exit.
- `u8 *flip = &D_801223F8;` gives `la s0` held across the display call.
- The CdControl result buffer and the upload timeout share sp+0x10: one
  function-scope `volatile s32 wait;` passed as the result (`void *`
  parameter) and used as the timeout (retail re-reads it after each store,
  so the volatile is justified). Block-scoped separate locals cannot share:
  assign_stack_temp only reuses a slot of the same mode, and a scalar whose
  address is taken later is put into a permanent slot.
- Recovery: `do { do { while (ready != 1) {} } while (pending ||
  (setloc(2, &RESTART, &wait), 0)); } while (!read(&RESTART, 0x1E0));`
  gives retail's two separate `la` of D_80122414. Putting setloc in the
  inner condition puts a LOOP_END note between setloc and read, and cse1
  stops there, so the two addresses are not merged into one hoisted pseudo.
  All other shapes (for/continue, do/while ||, nested do/while) compile to
  the same merged form.
- Frame wait: `retries = 2000; do { frame = update(&DISPLAY); if (frame)
  break; } while (--retries); if (frame) { handler; status = 0; } else
  status = -1; if ((s16)status != -1) break;` gives retail block order.

Remaining:
1. The update-frame call argument is `move a0,s3`: cse1 follows the
   threaded `bnez frame` jump into the handler and merges its &DISPLAY with
   the call argument, which loop.c then hoists (`global` movable). Retail
   loads `la a0` in the loop and keeps a separate hoisted handler base
   (`move s4,s1`). Tried while/for/do-while(!frame && --retries)/if
   (retries)/a display pointer local: no change.
2. Related: no `sll v0,v0,16` in the `j` delay slot of the timeout path,
   handler scheduling (`lw a2,PTR4; lhu DBC; lbu decodeIndex`), and
   finished/decode-copy registers swapped (s3/s4).
3. Upload wait loop: retail keeps absolute region/rect/done accesses and
   only `done = 1` and the regions base go through `la a0,&DISPLAY`;
   stock makes everything relative to a0 = &DISPLAY.done. A display pointer
   local makes it worse (hoists all addresses).
4. Finish test: retail copies `move v0,s3` before `bnez`, and
   `D_800B0DBA--` keeps the `la v1` form with the store in the jal delay
   slot.
