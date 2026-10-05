# scene_e19_2 func_80192F9C: blast sequence controller (parked)

Retail: `configs/USA/overlays/scene_e19_2.yaml` `[0x0, asm, func_80192F9C]`,
vram 0x80192F9C, 0x2500 bytes (2368 words), frame 0x330.

**Current: lev 93** (`SceneE19_BlastSequence.c` + `scene_e19_blast.h`, plain C,
three t4/t5/t6 transfer pins in the matrix-load macro as the only crutch).
Score from the worktree root:

    PATH=/Users/hasik/Projects/parasite-pc/tools/binutils-2.45/bin:$PATH \
    bash tools/scripts/cc.sh candidates/overlays/scene_e19_func_80192F9C/SceneE19_BlastSequence.c \
        /tmp/blast.o -Icandidates/overlays/scene_e19_func_80192F9C
    python3 <helpers>/lev.py /tmp/blast.o scene_e19_2 0 2500 --func func_80192F9C -v 40

`-DLEGACY` swaps the matrix macro for the legacy `gte_ldrotmatrix` /
`gte_ldtransmatrix` pair (not admissible, CPU `lw` in asm). It is only a
measuring aid: it shows what the rest of the function scores once the
matrix windows are exact, and the allocation it produces is retail's.
`-DPINB` adds a `$9` matrix-pointer pin, `-DPINC` the user's slot-address
barrier on top (both inadmissible, kept for comparison only).
Measured: admissible build lev 93, `-DLEGACY` lev 46, `-DPINC` lev 78.

`func_80192F9C.c` / `scene_e19_recovered.h` are the earlier m2c-derived draft
(nine pins, eleven barriers, two identity inline helpers; lev 169). It is
superseded by the rewrite and kept only as a reference; its long research log
is in git history (README before this rewrite, commit c11d70137).

## What the function does

mode 0 loads five shell models (archive keys 0xC54C0704 + n*0x400000) into
D_8019B680..D_8019B690, resets state/timer, snapshots the actor position,
target and origin, and starts an 8-slot spark pool (callback func_80192E08).
mode 1 is a five-state timeline: states 0..2 last 16 frames (state 0 plays
sounds 0x5E3/0x5E4 at frame 2), state 3 (32 frames) puts the actor into
animation 0x2D when it is in range, fades the screen and emits rising sparks,
state 4 (24 frames) keeps emitting and fades D_8019B668 out, then returns 1.
mode 2 draws per state: floor glows, an expanding shell model, a 16-blade
ring, flares at the target, and in states 3/4 four or five shell models that
lift and spin, then restores the sprite parameter block.

## Region status (lev edits by retail offset, admissible build)

| Region | Retail range | lev edits | Status |
| --- | --- | --- | --- |
| prologue / epilogue | 0x0, 0x24CC | 0 | exact (frame 0x330) |
| mode 0 | 0xCC-0x220 | 0 | exact |
| mode 1 | 0x220-0x710 | 7 | only branch offsets into the epilogue (size is 3 words short) |
| mode 2 head | 0x710-0x7C4 | 16 | matrix window (pointer load hoisted above the center copy) |
| draw state 0 | 0x7C4-0x8E8 | 0 | exact |
| draw state 1 | 0x8E8-0xCC0 | 10 | matrix window only |
| draw state 2 | 0xCC0-0x11BC | 49 | parameter/page base registers (about 38) + matrix window |
| draw state 3 | 0x11BC-0x1C80 | 11 | matrix window only |
| draw state 4 | 0x1C80-0x2440 | 0 | exact |
| tail | 0x2440-0x24CC | 0 | exact |

### Remaining causes

1. **Matrix windows (about 10 per window, five windows).** Retail is the PSY-Q
   macro shape `la v0,D_800BCFA4; lw t1,0(v0); nop; lw t4..` (pointer as the
   asm input's reload register). C word reads with the allowed t4-t6 pins give
   `lui v0; lw v0,%lo(D_800BCFA4)(v0)` and all word loads through v0. The
   la form needs the slot address to survive combine (two uses), and t1
   needs a pin; both are outside the rules (see scene_e20 candidate README for
   the same analysis). In the mode 2 head the folded pointer load is also
   hoisted above the three center copies (a symbol load never conflicts with
   stack stores; retail's load through the la'd register does).
2. **Draw state 2 parameter block (about 40).** Retail keeps
   `&D_800F3368` in s1, `&D_800E11FA` in s2, `D_800E2850` in s0 and
   `D_800E1204` in s7 across the CEE20 call and two `if (frame & 1)` joins,
   and stores palette/parameter06/tpage through 4/6/8(s1), reads
   `-16(s2)`. cse (both passes) knows these pointers are constants after the
   skipped-block joins and folds `(plus reg k)` addresses back to constants;
   an empty asm flush before the second block confirms the mechanism but
   then reload substitutes the constant because the "constant register"
   (REG_EQUIV, live length doubled) gets no hard register. No plain C
   form found yet; retail's constant 3 lives in fp across the call.

## Steps that moved the score (in order)

- Clean rewrite from the m2c draft (typed spark, blast, channel records; no
  pins/barriers): 493 (stripped draft) -> 342.
- Mode 1: per-state `state = n; timer = 0; break;` so jump2 cross-jumps the
  shared tail into state 3's block (retail `j L; li v0,n`); u16 spark
  coordinates for the `+= rand() % spread - 0x100` order.
- All parameter-block writes through `D_800F3368.field` (struct), tpage
  first, then palette, parameter06: draw states 0/1, every shell texture
  setup, the head (`parameter0A`, `depth`, keeps the state load below the
  stores) and the tail. 342 -> 166 (legacy scale).
- `-D_800E27EC << 5` / `-D_800E27EC * 0x30` (negate first).
- One variable for the ring blade length and the shell lift height
  (`height`), the ring radius 2000 in `modelPhase`, the ring angle in
  `verticalScale`: these give retail's s7/s1/s2 map. 
- `center.y -= timer * 0x18` instead of `center.y = height - ...`: the
  narrowed HImode subtraction created a HI copy of height that cse reused
  for the restore (extra `move` and an extra frame slot); the in-place form
  fixed the 0x330 frame.
- Ring z offset written in place (`ringPoint.z += cos * radius / 4096`): 124.
- Draw state 0 also goes through `phase = timer << 6` before its cosine
  (every other draw state does). Combine folds the copy into a0, but flow
  already counted the two references, which lifts phase's global-alloc
  priority (23 -> 25 refs, .116 -> .126) above the shared height (.122), so
  phase takes s6 and height s7 as in retail: 124 -> 93.
