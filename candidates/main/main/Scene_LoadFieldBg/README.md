# Scene_LoadFieldBg (main 0x5B540, 0x61C bytes): 42 real diffs

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
