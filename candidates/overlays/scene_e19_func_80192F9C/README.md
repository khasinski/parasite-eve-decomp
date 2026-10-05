# scene_e19_2 func_80192F9C: blast sequence controller (parked)

Retail: `configs/USA/overlays/scene_e19_2.yaml` `[0x0, asm, func_80192F9C]`,
vram 0x80192F9C, 0x2500 bytes (2368 words), frame 0x330.

**Current: lev 78** (`SceneE19_BlastSequence.c` + `scene_e19_blast.h`, plain C,
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
Measured (lev 93 state): admissible build lev 93, `-DLEGACY` lev 46, `-DPINC` lev 78. Current admissible build: lev 78.

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
| mode 1 | 0x220-0x710 | 7 | only branch offsets into the epilogue (build is 8 words short; see below) |
| mode 2 head | 0x710-0x7C4 | 20 | matrix window (pointer load hoisted above the center copy) + the two `la` of the draw-state-2 pointers |
| draw state 0 | 0x7C4-0x8E8 | 0 | exact |
| draw state 1 | 0x8E8-0xCC0 | 10 | matrix window only |
| draw state 2 | 0xCC0-0x11BC | 30 | missing `la s1`/`la s2` (set in the head instead), 336C/336E store order, `li s4,0x1000` slot, matrix window |
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

## Follow-up round (coordinator, after lev 93)

### Mode 1 branch offsets
All seven mode 1 edits are branch/jump offsets into the shared epilogue at
0x24C8/0x24CC; mode 1 itself is instruction-exact. The size gap is the sum of
the per-region insertions and deletions after 0x710: every matrix window is one
word short (retail `lui; addiu; lw t1; nop` vs `lui; lw`, plus the head window
order), and draw state 2 is 8 words long/short in places (the two pointer `la`
pairs now sit in the mode 2 head). There are no missing words in mode 1, so the
offsets line up only once the matrix windows and draw state 2 match; nothing
separate to fix there.

### Draw state 2 base registers (49 -> 30, total 93 -> 78)
- Siblings that keep `&D_800F3368` in a saved register (room_m318
  TwinRibbonFlow, room_m350 ConfiguredPulseCallback, room_lib
  SpawnRingController) all use an empty tied asm on the pointer; the
  barrier-free siblings (room_m273 FallingTrailFlow) come out absolute. No
  plain-C precedent.
- Mechanism: cse knows a pointer local is constant in the extended block where
  it is assigned (skipped `if (frame & 1)` blocks do not end it) and folds
  `(plus reg k)` addresses back to constants in both cse passes. Assigning
  `params = &D_800F3368; pageSelector = &D_800E11EA[8];` in the mode 2 head
  (after the matrix load, before the state switch) hides the constants from
  draw state 2 behind the tablejump: the stores become `sh ..,4/6/8(s1)`,
  `lhu -16(s2)`, `lhu 4(s1)` with retail's s1/s2 and fp = 3. Cost: the two
  `la` pairs are in the head instead of at the top of draw state 2 (8 edits).
  Function-top assignment gave 84, the head 78; `register` keyword no change.
- Remaining: 336C/336E absolute stores scheduled before the page read (fp
  constant), `li s4,0x1000` placement; 18 store-order/placement variants
  tried, none better.

### Jump tables and rodata layout
The function's own `.rodata` is now exactly retail's 0x8018F1D4..0x8018F210
(0x3C bytes): writing the three local initializers in C
(`GteRotation flareRotation = { 0x400, 0, 0, 1 }`, glow `{0x8C,0x8C,0x46,0}`,
flare `{0x8C,0x46,0x8C,0}`) makes GCC emit them as anonymous rodata copied
with lwl/lwr exactly as retail (lev unchanged), followed by the two tables and
the `.align 3` pad word at 0x8018F1F8:

    +0x00 flare rotation (8)   0x8018F1D4
    +0x08 glow color (4)       0x8018F1DC
    +0x0C flare color (4)      0x8018F1E0
    +0x10 state jump table     0x8018F1E4 (5 words)
    +0x24 pad word             0x8018F1F8
    +0x28 draw jump table      0x8018F1FC (5 words)
    end                        0x8018F210 (func_8019549C's initializers follow)

So the existing extern names D_8018F1D4/D_8018F1DC/D_8018F1E0 go away for this
unit (TwinGlow still references D_8018F1D4 through its own header as a shared
constant; check its retail references before removing the sym).

Repo convention: a C unit whose rodata lives in the scene header is carved as
`[off, .rodata, Unit]` inside the header segment and `[off, c, Unit]` in the
code segment of the **same** yaml (scene_e09 RoomEffect_DroppedFlareParticle,
0x20C and 0x4F7C). The splat linker script then links both sections from one
object, and everything else falls into `/DISCARD/ : { *(*) }`. Across two yamls
this cannot work: scene_e19's link would keep the `.rodata` but discard the
`.text` its table entries point at (and vice versa for scene_e19_2), which GNU
ld rejects ("defined in discarded section"). No other `_2` overlay exists, so
there is no cross-binary precedent.

The two slices are contiguous in PE.IMG (scene_e19: word 50477568, 4077 words;
scene_e19_2: word 50481645 = 50477568 + 4077, 8723 words); together they are
0x3FB4 + 0x884C = 0xC800 bytes = exactly the 25 sectors of section 3. The
needed change is therefore to merge scene_e19_2 back into scene_e19:

1. configs/USA/overlays/scene_e19.yaml: replace the trailing `- [0x3FB4]` with
   the three scene_e19_2 code segments, every `start` shifted by 0x3FB4 and
   the vram kept: `scene_pre` start 0x3FB4 vram 0x80192F9C, `scene_main` start
   0x64B4 vram 0x8019549C, `scene_accessors` start 0xA1F0 vram 0x8019B1D8 with
   its `[0xA26C, data]`, then `- [0xC800]`. The scene_e19_2 C subsegments keep
   their names; their sources must sit under the merged src_path
   (src/overlays/scene_e19) or use dict-form subsegments with
   `dir: scene_e19_2` (the header rodatabins already use `dir:`).
2. Split the header tail rodatabin at the unit's rodata:

        - start: 0x1E4
          type: rodatabin
          name: scene_e19_header_tail
          dir: scene_e19
        - [0x1EC, .rodata, SceneE19_BlastSequence]
        - start: 0x228
          type: rodatabin
          name: scene_e19_header_tail2
          dir: scene_e19

   and in scene_pre `- [0x3FB4, c, SceneE19_BlastSequence]` (the unit name
   replaces func_80192F9C, symbol stays func_80192F9C).
3. Extraction comment: one `dd ... bs=4 skip=50477568 count=12800` (the full
   section) instead of the two slices; regenerate original/USA/overlays/
   scene_e19.bin, delete scene_e19_2.yaml and its linkers/undefined files, and
   bump the private assets pin (CI) since the target bins change.
4. Move include/pe1/scene_e19_2_*.h users unchanged (headers are not
   path-bound); progress/match inventory entries for scene_e19_2 move under
   scene_e19 (docs/match_inventory.json, SOURCE_QUALITY.md, the extraction gap
   doc).

Until the function reaches lev 0 the merge can be done independently with the
function still as `[0x3FB4, asm, func_80192F9C]` and the header tail left as
one rodatabin; overlay-check then compares one 0xC800 binary instead of two.
