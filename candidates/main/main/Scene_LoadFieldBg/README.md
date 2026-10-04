# Scene_LoadFieldBg (main 0x5B540, 0x61C bytes): lev 42

Plain C: no pins, barriers, aliases, pointer/integer casts or byte-pointer
arithmetic. The six read/poll stages use `goto retryN` (6 gotos, counted
debt). Apply `headers.diff` (names the loader's Pe1GameState fields,
makes `banks[]` signed as CD_SeekToTrack already treats them, makes
Asset_FindTable08ByU32Key return `void *`, adds include/pe1/field_bg_load.h),
copy the .c to src/main/main/ and score with offset 5B540 size 61C.

What already matches (everything but two insns modulo s-register names):
- Retry stages as `retry: while (Read(...) == -1); status = 1; do { if (!done)
  {...} if (status == -1) goto retry; status = CdRom_PollReady(); } while
  (status != 0);`. A real outer loop instead of the goto hoists the -1 and
  the sector-table address out of the restart path (retail rematerialises
  them after the label).
- Function-scope `status`, `done`, `blob`, `directory`, `tim`; the stage loop
  index is block-scoped per TIM loop so `done` is the canonical register of
  the zero class and the entry test stays `sltu v0,done,count`.
- The tail TIM loop has its own block-scoped `blob` (48 -> 42 diffs): that
  drops the function-scope directory below status in global-alloc order, so
  directory now gets retail's s3 everywhere. The tail blob pseudo follows the
  function-scope blob through a copy preference.
- The tail loop needs `i = 0; if (count) do {...} while (++i < count)`.
- `&tim[i]` (not `tim++`), libgpu getTPage/getClut packing written out,
  early `return 0` for retail's tail.

Remaining (cc1 -dl/-dg, priority = floor_log2(refs)*refs/live_length):
1. status vs blob swap: retail status s2, blob s4; mine blob s2, status s4.
   status 42 refs / 166 insns = 1.265, function-scope blob 24 / 68 = 1.41,
   so blob is allocated first. Every block-scoped split of blob (stage 2,
   stage 6 or all) produces short high-priority pseudos (11/22 = 1.5) that
   also beat status. Retail needs status ahead of every blob pseudo.
2. Stage 5 (three Asset_FindTable08ByU32Key stores): retail schedules
   `done = 1` into the first jal delay slot; stock sched1 puts it before
   the `lw a0` and fills the slot with the `ori a1`.
Tried without effect: all 64 block/function scope combinations of blob,
directory and tim in the stage macro and the tail (best 42), per-stage
status/done blocks (worse), while-form poll loops, separate tail locals.

Update (agent 6, 2026-10-04, lev.py): lev 42 on current main. headers.diff
no longer applies cleanly: Scene_LoadEntityTextures renamed 0x11C to
`scene_object_model` and 0x158/0x15C to `entity_texture_blob` /
`scene_object_work`; merge by keeping those names and adding
room_geometry_table (0x120), bg_texture_blob (0x160),
object_texture_blob (0x16C), bg_tim (0x174), hud_tim (0x180), and use
`scene_object_model` in the .c for the 0xC4B5BA04 table.
Global-alloc order from cc1 -dl/-dg (priority = floor_log2(refs) *
refs / length): blob 18/53 = 1.358 -> s2, directory 30/89 = 1.348 -> s3,
status 42/166 = 1.265 -> s4. Retail order is status, directory, blob
(s2, s3, s4), so status must beat both: 47 refs at the same length, or a
length under ~154. 40 of status's refs come from the poll do/while depth
weighting; the body inside the poll loop keeps status live across it. A
block-local blob in stage 6 (lev 42, unchanged) and the function-scope
blob in the tail (lev 48) were retried.
