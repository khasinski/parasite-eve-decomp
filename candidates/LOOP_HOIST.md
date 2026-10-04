# Loop-invariant hoisting in stock GCC 2.7.2 loop.c (agent 5, 2026-10-04)

Study of the "retail keeps a constant inside the loop, stock hoists it" blocker
that parked room_m075/m080/m082 func_8018F3DC, room_m273 func_8019665C,
room_m273 func_801981A4 and room_m350 func_80192E4C.

**Conclusion: this is not a compiler difference. Stock loop.c reproduces every
retail loop once the C has the right shape.** The earlier claim that SN's
threshold is "about 24 instead of 58", or two registers lower, came from
candidates whose loops had the wrong insn count or the wrong movables. Do not
park a function for this reason. Read the `-dL` dump and change the source.

## The rule (loop.c `move_movables`)

    threshold = (loop_has_call ? 1 : 2) * (1 + n_non_fixed_regs)   /* 29 with a call, 58 without */
    move if  threshold * savings * lifetime >= insn_count
    threshold -= 3 after every move (matched "done" moves do not lower it)

- `insn_count` is the "N real insns" line of the dump. It counts insns as they
  are when loop runs: after cse1, before combine. An insn that combine
  deletes later still counts here.
- Movables are visited in insn order. Every earlier move lowers the threshold
  for the later ones.
- A loop that contains a call starts at 29, not 58. Most "retail keeps
  `li 1` for the last stack argument" cases are call loops.

Dump it with `cc1 ... -dL` (writes `<file>.i.loop`). Each line
`Insn N: regno R (life L), ... savings S moved to` / `not desirable` gives the
decision. Work out the threshold at each movable from the moves before it.

## Three levers, all plain C

1. **Add a movable before the constant that gets hoisted and then vanishes.**
   An explicit `(s16)` cast on an int argument emits a sign-extension pair
   (ashift/ashiftrt). The pair is invariant, gets hoisted (-3 or -6 off the
   threshold) and adds insns to the count. Later, combine removes it once
   `num_sign_bit_copies` shows the value already fits in 16 bits (`lh >> 5`,
   or `0x80` or `cos >> 5`). It can also leave the plain register copy that
   retail shows (`move s4,s2`).
2. **Pass a literal instead of a variable assigned before the loop.** A literal
   needs a `(set reg const)` inside the loop, which is one more counted insn
   and one more movable. A variable set outside the loop is neither.
3. **Use a pointer into the record (`p = &table[i]; p->f = ...`)** rather than
   indexing the array in every statement. The insn count and the movables
   change. This form also gives retail's `addu v1,a3,base`, with the base
   hoisted and the second base `la` left inside the loop.

## Evidence per function

### room_m273 func_801981A4 (sway shard callback): MATCHED

Two-flare loop around func_800D0728 (11 args).
- Old candidate: `scale = 0x1000;` before the loop, args `scale, scale`. Dump:
  `23 real insns`. &spin moved (29), D_8019AE04 moved (26), D_8019AB70 moved
  (23 >= 23). Retail keeps the last two inside.
- Fix: pass the literal `0x1000, 0x1000`. Dump: `24 real insns`. &spin moved
  (29), 0x1000 moved (26, life 2), D_8019AE04 at 23 < 24 is
  `not desirable`, and the same goes for D_8019AB70, 0x80 and 1. This is
  exactly retail: `li s1,0x1000` and `addiu s5,sp,0x38` sit before the loop.
  The `la s4,D_8019AB70` / `li s3,0x80` / `li s2,1` stay in the loop body, and
  cse2 reuses s2..s4 after the loop.
- The other diff was a `move v1,v0` in the trail frame. It was fixed with
  `s16 frame = D_800E27EC & 7;`.

### room_m350 func_80192E4C (sweeping beam trap): MATCHED

Fan blade loop around func_800D0E88 (call loop, 9 args, last one `1`).
- Old: `22 real insns`. D_8019A43C, D_8019A3C8 and the `1` were all moved, so
  `li s4,1` sat outside the loop.
- `(s16)intensity` as the 8th argument: regno 272 (ext, savings 2) and 271
  were moved and lowered the threshold to 17. `Insn 613: regno 273 (life 1)
  ... not desirable` is the `1`. The loop is byte-identical to retail,
  including retail's `move s4,s2` copy of the intensity, which is the
  extension that combine reduced to a copy.
- Fixes for the other diffs in the same session (all plain C):
  `D_8019A444[2].z = D_8019A444[3].z = -dist;` (retail keeps the &[2].z
  register). One `GteShortVector unused;` in the final node block gives the
  frame size. Re-reading `trig = D_800966EC;` before the glow block and using
  `trig[...]` for both glow lookups puts count/trig in s3/s1 and the spin.x
  store first.
- Remaining 2 diffs: `addiu t0,sp,0xB0` (&outside, hoisted from the nclip
  loop) and `addiu a2,sp,0x70` (vertex = quad) are swapped. This is an sched
  tie-break between two independent insns. Reordering the
  player/prev/k/vertex statements did not change it.
- Fix: index the quad (`quad[k].z`, `quad[k].x`) instead of walking a
  `vertex` pointer set before the loop. Strength reduction then creates the
  pointer giv and emits its init at the loop start AFTER the moved
  invariants, so `&outside` comes first, as in retail.

### room_m273 func_8019665C (queued drop callback): MATCHED

Two-ring loop around func_800D0E88 (call loop).
- Old: `19 real insns`. D_8019AD58 (29), D_8019AD54 (26) and the `1` (23 >= 19)
  were all moved.
- `(s16)shade` as the intensity argument: `21 real insns`. AD58 and AD54 are
  moved, then the extension (regno 221 savings 2, plus regno 220
  `cond forces`). The `1` is then `not desirable` (17 < 21). The loop
  instructions now equal retail's, and combine folds the hoisted extension
  away because shade = `lh >> 5`.
- The frame needed one unused 8-byte local (`GteShortVector unused;` after
  `ring`).
- Remaining (41 word diffs, all one s0<->s1 swap): retail puts the loop index
  (and the flash-branch size) in s0 and `drop` in s1. Global-alloc priority
  (`floor_log2(refs) * refs / live_length`, from `-dl`) is 31 refs / 126
  insns = 0.98 for drop and 7 / 21 = 0.67 for i. Sharing i with size gives
  9 / 35 = 0.77, which is still lower. Only loop-depth weighting
  (`reg_n_refs += loop_depth`) would lift i, and a `do { } while (0)` around
  the mode 2 branch does exactly that (permuter score 45). That wrapper is
  banned. A `RoomM273Drop *d = drop;` copy for mode 2 creates a second pseudo
  and costs a move.
- Fix: share one function-scope `int i` between the loop index and the
  mode 1 landing slot (`i = D_8019AF04.count++; D_8019AF04.x[i] = ...`).
  The slot adds refs on a short live range, which lifts i's global-alloc
  priority above drop's, so i gets s0 and drop s1 as in retail.

### room_m075/m080/m082 func_8018F3DC (motion particle init): MATCHED

- The old draft (scratchpad m75i.c) used register pins, byte-offset pointer
  arithmetic (`(char *)D_801940C8 + i`) and volatile globals. Its loop hoisted
  -0x40. `-ffixed-t8 -ffixed-t9` "fixed" that only by accident.
- Clean form: `RoomMotionParticle D_801940C8[10], D_80194370[10]` (0x44-byte
  records), `unsigned int i` from 0 to 10, and `particle = &D_801940C8[i];
  particle->... ; particle = &D_80194370[i]; ...`. Dump: `49 real insns`.
  base/0x80/4/30 are moved (threshold 58 -> 46). `-64` (regno 173, life 1) is
  `not desirable` at 46 < 49, and 0xFF is moved (life 6, savings 2). Strength
  reduction turns i into the byte offset a3 with `sltiu 0x2A8`. **The loop is
  byte-identical to retail, registers included (t4, t3, t1, t2, t0, a3), and
  `li 4` / `la D_80194370` / `li 64` stay inside.**
- The four 16-byte glyph records at 0x80194618 must be one array
  `RoomMotionGlyph D_80194618[4]`. As four separate symbols, local-alloc gives
  the constants other registers.
- Remaining (38 word diffs, one cause): `D_80194618[2].y = 32` (`sh v0` to
  0x80194642) is scheduled as the first glyph store, and `li v0,-31` follows
  it at once. Retail stores it in source order. Moving the statement or
  swapping x/y did not help.
- Fix (sched2, not loop): the `li v0,-31` competes with ready stores, and
  stores always win the potential-hazard tie, so the -31 load sank to the
  top and dragged the [2].y store with it (v0 anti dependence). Writing
  `D_801940B8.x = -31; D_80194618[3].size = 32;` after `D_80194618[0].y = 128`
  creates the HImode 128 pseudo first (lower LUID), so `li a0,0x80` is
  scheduled first and the -31 load lands right before its store. Lesson:
  when a constant load floats far up in a long store run, move the
  statement that creates it later in the source.

### main Render_SetupEntityPrims texture loops (shared cursor copy): loops MATCHED

- Retail copies the section cursor into t0 at the top of each texture loop
  (`move t0,a1`), reads the record through t0 with raw offsets, and steps
  the cursor in the back-branch delay slot (`addiu a1,a1,16`). A plain
  `srcquad = cursor.quad; ...; cursor.quad++;` lets loop.c treat srcquad
  as a DEST_REG giv of the cursor biv. Every `srcquad->field` load becomes
  a DEST_ADDR giv, combine_givs folds them into the last one, and the
  result is reduced (`addiu t0,a1,6`, offsets -5..7).
- loop.c only marks a DEST_REG giv replaceable (which lets it derive the
  address givs) when `regno_first_uid` is the giv's own insn, so the
  variable must not be set anywhere before the loop. Fix: one
  `RenderModelCursor src` for both loops, first set before them where it
  has a real use (`src = cursor; *textureOut = src.commands;`, the texture
  section start), then `src.quad = cursor.quad;` at the top of each loop
  and `cursor.quad++` at the end. Both loops become byte-identical with no
  dead store.
- Forms that fail: `cursor.quad = srcquad + 1` (cursor stops being a biv,
  but global alloc merges srcquad and cursor into one register),
  `srcquad = cursor.quad++` (still reduced, `t0 = a1 - 10`), the copy
  inside the inner loop (hoisted, still a giv), and walking the copy itself
  as the biv. A dead second assignment (`srcquad++` after the inner loop)
  also works, because a register set twice is never a giv, but it is a
  steering store.

## What did not work

- Statement order alone. Loop decisions depend only on the movable order and
  the insn count, not on where the arguments sit relative to other code.
- `-ffixed-*` style threshold changes. This is not a compiler-config
  difference, and per-file flags are not allowed anyway.
- An s16 local for the intensity/shade. It is extended at assignment, outside
  the loop, so it adds no movable. The cast has to sit in the call.

## Register priority from references that later disappear (2026-10-04)

- flow computes `reg_n_refs` and `reg_live_length` before combine runs, and
  combine does not lower them when it folds a reference away. Global
  allocation sorts by `floor_log2(refs) * refs / live_length`, so an
  assignment that combine later removes still raises that variable's
  priority.
- Scene_LoadRoom: `while ((ready = CdRom_ReadSectorsFromLba(...)) == -1)`
  produces exactly the same code as the bare call, because combine compares
  v0 directly. But `ready` keeps the extra counted references, so it is
  allocated before the loop counter `i`. That fixed a s1/s2 swap worth about
  40 words.
- Menu_ItemListInputHandler: `usable |= 1` gives `usable` one more counted
  use and puts usable, child and data in retail's registers (the cost is an
  `ori` instead of `li`). The match came from the other side: reusing
  `child` for the final equipment-node lookup of the same branch (where
  `child` is dead) gives it 3 more refs, so child (s1) is allocated before
  data (s2), and the plain `usable = 1` then fits in s1 next to child. When
  one pseudo must beat another, raise the loser's competitor instead of the
  variable itself: reuse a dead pointer of the same type for a later lookup.
- Frame side effect: when combine simplifies a `for` loop entry test
  (`0 < n` to `n != 0`), it leaves a dead `sltu` pseudo behind a USE insn,
  and reload gives that pseudo an 8-byte stack slot that is never used.
  Count the `(use (reg:SI N))` lines in the `-dc` dump. Writing one loop as
  `i = 0; if (n) do { ... } while (++i < n);` removes one slot and leaves the
  code unchanged.

## Operand order and copy shape fixes (Scene_LoadRoom, 2026-10-04)

- `addu` operand order for a pointer-variable table: `PmCommand **p =
  &g_PmCmdHandlerTable[slot]; if (*p == 0) *p = x;` expands as a normal
  binop (scaled index emitted first, table loaded by force_not_mem, sum
  ordered table + index). Indexing the table directly in both the test and
  the store (`if (g_PmCmdHandlerTable[slot] == 0) g_PmCmdHandlerTable[slot]
  = x;`) goes through the address path instead and gives retail's
  index + table order.
- `lhu v0; move v1,v0; andi v0,v0,1`: the key copy must be an SImode copy of
  an SImode load. `int key = field; if (field & 1)` loads HImode and
  zero-extends (`andi v1,v0,0xffff`); `if (key & 1)` drops the copy. Testing
  `(u16)key & 1` keeps the load SImode and the copy plain.

## Near-tie flipped by a reused variable (Render_InitDisplayLists, 2026-10-04)

- state (33 refs / 222 insns) and wait (41 / 278) were within 0.01 of
  each other in global-alloc priority. With retail's statement order the
  wrong one won. After the display loop `wait` is dead, so
  `while ((wait = Cd_GetReadyStatus()) != 1) VSync(0);` reuses it for the
  drive status: combine compares v0 directly (no code change), but flow
  counted the extra references, which lifts wait above state. Same
  mechanism as `ready` in Scene_LoadRoom.

## Two registers for one incoming pointer (scene_e08 func_8019104C, 2026-10-04)

- Retail kept the third argument in s1 for the `ticks < 12` block and in a
  second register (copied before the test) for the rest, with s1 reused for
  &matrix afterwards. No plain copy survives cse: on the path that skips the
  block, the older pseudo stays canonical and the copy's uses are rewritten.
- The copy survives when the incoming pseudo is assigned again at the start
  of the second half: declare the argument `void *data`, read it through two
  typed locals (`ring = data; state = data;`, ring for the block, state for
  the rest), and reuse it as the matrix pointer (`data = &matrix;`). The
  reassignment ends the equivalence on the skip path, and the incoming
  pseudo outlives the copy, so the copy never becomes canonical on the
  block's path. Global allocation then gives the multi-set incoming pseudo s1
  across both halves, which is retail's register use.

## Slice upload (menu_memcard func_801214D4 and Memcard_UploadVideoSlice)

- This is the PSY-Q movie sample slice callback. Written with direct struct
  accesses (`old = dec.selector; rectangle = dec.rect; dec.selector ^= 1;
  dec.rect.x += dec.rect.w; ...`) it matches without volatile: the BLKmode
  rectangle copy between the two selector reads invalidates memory in cse,
  so the selector is read twice, and the forced constant-address pseudo for
  the selector is what keeps `la a3` and `buffers[...]` at -8(a3).
- Field symbols overlapping a struct (the twin's D_801D148C etc.) and pointer
  locals to the struct fields made things worse: a local pointer is a known
  constant to sched, so later loads hoist above its stores.

## Path sampler (fx_common func_8018F55C, 2026-10-04)

- `lhu; sll 16; sra 16` into a saved register is a u16 field read into an
  `s16` local. A signed field, or an `int` local, gives a bare `lh`. The
  header record is now `FxCommonPathPoint { s16 x, y, z; u16 count; }`, so
  no volatile is needed.
- In-place product (`delta = nextYaw - yaw; delta *= fraction; ...
  angles[1] = yaw + (delta >> 8);`) keeps the subtraction and the mflo in
  one register. That fixed the last v0/v1/t3 swap, the same effect as the
  `d *= d` lesson.



## Stat level bar (main Draw_AllocTexturedRectAlt, 2026-10-04): MATCHED

- Pointer-free OT link: give the packet tag a `u32 address : 24, length : 8`
  bitfield view and write `quad->tag.link.address = *ot;` (the bitfield
  insert is exactly retail's and/and/or with the two masks), then pass the
  packet pointer through a `union { RenderTexturedQuad *quad; u32 word; }`
  for `*ot = (*ot & 0xFF000000) | (link.word & 0xFFFFFF)`. lev 175 -> 67.
- Retail allocated each slice into s0 (live across the assert call) and
  filled it through a3 (`move a3,s0` at the join, duplicated into the
  branch delay slot by reorg). A function-scope `quad` assigned from every
  allocation keeps the copy: cse makes the longer-lived register canonical,
  so the fills use the copy. lev 275 -> 175 (with the link change).
- `(s16)quad->x0 + 0x2E - width` keeps retail's `(x0 + 46) - width`; on
  the bare u16 field fold rewrites it as `x0 - (width - 46)`.
- Read `glyph->mode` into a local before the last allocation when retail
  loads it into the dying glyph register ahead of the arena check.
- A v1/a0 swap between the left edge and the x0 read-back in five slices
  was the chained store `quad->x0 = quad->x2 = left;` (two separate stores
  of the same expression give the other local-alloc order).

## Hoisting shifts of an HImode local (fx_common func_80193B5C, 2026-10-04)

- Locals are not promoted in this GCC (mips.h has no PROMOTE_MODE), so an
  `s16` local stays an HImode pseudo and every use expands its own
  `sll 16; sra 16`. Retail's `sll t0,16; srl s2,t0,17; srl t3,t0,19` with
  the `sll` value also tested in the loop comes from writing the shifts
  inline at each store (`p->r = level >> 1;`) inside the loop, with no
  variable. loop.c moves non-user temporaries even out of conditional
  code (condition 2 in scan_loop); a user variable (`u8 half = ...`) set
  after the `continue` test is never moved, and one set at the loop top is
  moved but then shares its `sra` with the in-loop zero test.
- A union "address" variable reused for several OT links lives across the
  whole loop, so cse makes it the canonical register and the packet
  pointer copies collapse. Use one link union per packet.
- Keep `p++` in place followed by a copy (`line++; buffer->data = (u8
  *)line; quad = (Quad *)buffer->data;`): with `quad = line` alone combine
  folds the increment into `quad = line + 16`.
- reg_live_length used by global alloc is recomputed after sched1, so
  moving a copy statement in the source does not change its priority.

## Scalar reads vs packet stores (fx_common func_80193B5C, 2026-10-04)

- sched.c true_dependence exempts a fixed-address scalar read from an
  earlier in-struct store through a pointer only when the store is not
  QImode. So `lh D_xxx` may move above `sh`/`sw` packet stores but never
  above `sb` colour/uv stores. Read retail's load placement with that in
  mind before reaching for struct views of the scalar.
- A store through a plain `T *p` (no PLUS in the address tree, `*p` or
  `*p++`) is not in-struct, so a later scalar read stays below it. This
  reproduces the PSY-Q `*(long *)&p->x0 = sxy` idiom without a cast.
- Bitfield link writes (`p->tag.bits.address = ot->bits.address;`) emit
  the same and/and/or as the masked form but count extra uses of the
  0xFFFFFF pseudo before combine, which raises its global-alloc priority.
- A tie between two loop-long pseudos can hinge on a short-lived one's
  sched1 live length: moving one store of `y` later (sched2 restores the
  final order) shifted y below the hoisted shift value.

## Effect marker draw (fx_common func_80193B5C, 2026-10-04): MATCHED

- A 24-bit OT link written as a bitfield copy
  (`p->tag.bits.address = ot->bits.address`) uses the 0xFFFFFF pseudo twice
  (extract mask and insert mask) until combine folds them, so flow counts
  one more reference than the hand-masked form. Use it when a hoisted or
  prologue mask needs one more counted use to win a global-alloc tie.
- Stack outputs of a projection call (RotTransPers3-style `long *sxy`)
  are best declared as separate scalars. A struct or array makes every
  read in-struct, and an in-struct frame read keeps a true dependence on
  earlier in-struct stores through pointers; that dependence feeds the
  load latency into the priority of every value copied from it, which
  pulls those stores to the end of the sched1 block. A scalar frame read
  passes non-QImode in-struct stores, so source order decides again.
