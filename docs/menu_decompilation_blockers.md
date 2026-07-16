# Menu decompilation blockers

This is a working inventory of problem categories that are currently slowing or
blocking plain-C, byte-matching decompilation of the menu system.

Scope: main executable menu-related functions, especially the remaining
`asm, menu/...` entries in `configs/USA/main.yaml`. This document intentionally
does not propose whole-function asm, inline asm, register-asm, postpass, or
build-hack solutions. Old sources may still contain historical asm/register
levers, but new promoted decompilations must match as plain C.

## Current Snapshot

Remaining `asm, menu/...` functions at the time of this note:

| Function | Size | Main risk |
| --- | ---: | --- |
| `Menu_MainUpdate` | `0x2194` | huge state machine, repeated absolute writes, global reload semantics |
| `Menu_ItemListInputHandler` | `0xC1C` | large input/action state machine, unaligned 0x20-byte record copies |
| `Menu_InventoryInputHandler` | `0x3AC` | input dispatch and widget focus transitions |
| `Menu_InventoryPageNavHandler` | `0x2E4` | navigation state, widget cursor/scroll fields |
| `Menu_DrawBonusPointSpendPanel` | `0x2D4` | UI draw arithmetic and BP state globals |
| `Menu_DrawItemDetailPanel` | `0x268` | item data layout and draw branching |
| `Save_InitMetadataState` | `0x24C` | inlined text-window init/copy, register allocation mismatch |
| `Menu_OpenEquipScreen` | `0x248` | widget construction and equip-state layout |
| `Menu_CheckItemAffordable` | `0x228` | item usability rules, battle/equip state, special-case arithmetic |
| `Menu_DrawExpReviewPanel` | `0x210` | exp/BP animation state and global aliases |
| `Menu_StatSlotInputHandler` | `0x18C` | BP stat selection input |
| `Menu_CreateNotificationDialog` | `0x15C` | callback/widget construction, text buffer sizing |
| `Menu_DrawEquipStatsDelta` | `0x154` | stat layout and signed arithmetic |
| `Menu_OnEquipConfirm` | `0x14C` | equip mutation flow |
| `Menu_DrawAmmoTypeHeader` | `0x134` | widget field layout and draw-tail positioning |
| `Menu_InventoryPageInputHandler` | `0x128` | exact size achieved, wrong callee-save allocation |
| `Menu_FindSelectedEquipSlotItem` | `0xD8` | stack/reload ordering trap |
| `Menu_DrawSelectableEquipSlotRow` | `0x74` | previous register-asm match is not acceptable under the current plain-C rule |

Recently solved cleanly:

| Function | Size | Useful lesson |
| --- | ---: | --- |
| `Menu_DrawEquipSelectionList` | `0xF4` | filtered inventory list state is tractable when control flow is narrow |
| `Menu_DrawArmorItemDetail` | `0x114` | item-table resolution and display-name fallback are now understood |
| `Menu_InitBonusPointAllocState` | `0x164` | save-data/BP layout became concrete enough to match in plain C |

## Problem Categories

### 1. Callee-save register allocation and live-range shape

This is the most common hard blocker once the C is semantically right.

Observed cases:

- `Menu_InventoryPageInputHandler`: a pure-C reconstruction produced the exact
  target size `0x128` and the same behavior, but GCC assigned:
  `s3 = child`, `s0 = node`, `s1 = flags`, `s2 = handled`.
  The target wants:
  `s0 = child`, `s1 = node`, `s2 = flags`, `s3 = handled`.
- `Save_InitMetadataState`: a pure-C reconstruction got to `0x224`, then
  `0x23C` after fixing small-data classification, but target is `0x24C`.
  The remaining mismatch is mainly GCC choosing an extra `s1` cursor where the
  target keeps cursor/text-copy flow in `a0/a1/s0`.
- `Menu_DrawSelectableEquipSlotRow`: previous attempts only matched when locals
  were pinned with `register ... asm("$16")` / `asm("$17")`. Under the current
  policy that is a useful diagnostic result, but not an acceptable promoted
  decompilation.

Why this is hard:

- GCC 2.7.2's global allocator is sensitive to tiny source changes, but not in
  a predictable high-level way.
- `register` without explicit register binding often has no effect.
- Adding a local can fix one live range and break another.
- The byte target often preserves a local across calls in a specific `sN`
  register, while equivalent C gives a different callee-save order.

What helps:

- Rewrite source to shorten or extend one variable's lifetime rather than
  adding dummy variables.
- Split semantically independent paths with `goto` labels when target has a
  single shared epilogue.
- Keep target register roles written down before iterating. For example:
  `InventoryPageInputHandler`: `s4=root`, `s2=flags`, `s0=child`,
  `s1=page node`, `s3=handled`.

What is useful only for diagnosis:

- Explicit register asm can prove that a mismatch is allocator-driven, but the
  accepted source still needs a plain-C structure that reaches the same target.
- Empty inline asm or operand barriers can identify a lifetime/scheduler issue,
  but they are not acceptable in promoted code.

What remains out of scope:

- Whole-function asm bodies.
- Postpass reshuffling.
- New build hacks.

Severity: high. This is the main reason small functions sometimes remain asm.

### 2. Delay-slot and scheduler-sensitive source shape

Many target functions depend on the exact instruction placed in a branch or call
delay slot.

Observed cases:

- `Menu_InventoryPageInputHandler` uses `andi flags, 0x1000` in the delay slot
  of the `bnez handled` path, so both handled and non-handled flows reuse the
  same computed value.
- `Menu_DrawArmorItemDetail` matched only after shaping the item-table branches
  so that candidate assignment and jumps landed in the same places as target.
- `Menu_InitBonusPointAllocState` needed the loop order to become:
  `lh`, pointer increment, argument setup, query pointer increment, stat++,
  `jal`, store delta in delay slot.

Why this is hard:

- A normal C `if` can be semantically identical but put a `nop`, a move, or a
  store in a different delay slot.
- GCC 2.7.2 can move safe-looking loads/stores across calls and branches in ways
  that are very hard to steer without asm.

What helps:

- Prefer explicit `goto` to shared labels when target has a single ret path.
- Keep loop increments near their target instruction order.
- Snapshot a computed condition into a local only when the target clearly reuses
  the same register value.

Severity: high for byte-match, medium for understanding.

### 3. Small-data versus absolute-address classification

Wrong symbol declaration changes `lw/sw %gp_rel(...)` into `lui/addiu`, or the
reverse. That alone can change size, register pressure, and scheduling.

Observed cases:

- `Save_InitMetadataState`: declaring `D_800C1F80`, `D_800C0E44`, and
  `D_800A76A4`-family globals as scalar objects made GCC emit gp-relative
  accesses. The target uses `lui $at, %hi(...)` absolute stores. Declaring them
  as arrays moved the C closer to target.
- `Menu_InitBonusPointAllocState`: using separate globals like
  `g_AyaSaveLevel` and `g_AyaHpMax` produced separate materialized loads.
  Modeling the save block as a struct at `D_800C0E00` got the exact target shape.

Why this is hard:

- The same address range may be accessed both as named small data and as an
  absolute block in different functions.
- Existing C sometimes uses alias declarations with `__asm__("symbol")`. Treat
  them as evidence of symbol materialization first; only use a new alias if a
  named symbol/prototype solution cannot produce the target shape.

What helps:

- Use incomplete arrays for objects that must not be small-data scalars.
- Use a struct for a real data block when target loads multiple fields through
  one base register.
- Check `build/USA/main.map` before choosing symbol declarations.

Severity: medium-high. Usually solvable, but it can cascade into register
allocation differences.

### 4. Incomplete menu widget and state structure layout

The menu code is heavily field-offset driven. Unknown offsets make C noisy and
cause wrong load/store types.

Known improvements:

- `MenuWidgetNode` offsets are now much better understood:
  `cursor_x +0x44`, `cursor_y +0x48`, `x_limit +0x54`,
  `y_limit +0x58`, `scroll_y +0x5C`, `field_60 +0x60`.
- `Menu_InitBonusPointAllocState` confirms save-data fields:
  `D_800C0E00 +0x00 total exp`, `+0x06 HP max`, `+0x0A level`,
  `+0x10 bonus points`, `+0x28 stat array`.

Still weak areas:

- Item/action menu globals around `D_8009CF0C` through `D_8009CFD4`.
- Item records used by `Menu_ItemListInputHandler`, especially the 0x20-byte
  copy into `D_800A1960`.
- Battle/menu active entity state used by `Menu_MainUpdate`:
  `D_8009D254`, `D_8009D278`, `D_8009D1E8`, `D_8009D2E8`.
- Draw-buffer records addressed through `D_8009CDDC` and `D_800B0158 + slot*0x48`.

Why this is hard:

- A wrong signedness or width (`lh` vs `lhu`, `lb` vs `lbu`) breaks byte-match.
- A field modeled as `int` where target uses `short` changes arithmetic and
  sometimes delay slots.
- Some globals are not single variables but windows into packed runtime state.

Severity: high for large functions.

### 5. Inlined helper logic instead of helper calls

Some functions duplicate logic that already exists as a separate helper. Calling
the helper is semantically clean but cannot match.

Observed cases:

- `Save_InitMetadataState` contains two inlined copies of the same pattern in
  `Save_LoadMetadataWindowText`: fill active text window with `0xFF`, choose
  `Str_LookupTable8(record[4]-1)` or `Str_LookupTable4(0x1E)`, then copy bytes
  until `0xFF`.
- A normal helper call would be much shorter, but target has the body inlined
  twice, including repeated reloads of `D_8009D0C0` and `D_8009D0C4`.

Why this is hard:

- To match, C must duplicate source code even when the repo already has a helper.
- Once duplicated, GCC may still allocate cursor pointers differently.

What helps:

- Local macro-like source duplication, but as normal C statements.
- Avoid creating a new helper unless target actually calls one.

Severity: medium-high.

### 6. Unaligned struct copies and `lwl/lwr` sequences

Some data copies are not normal aligned word copies.

Observed cases:

- `Menu_ItemListInputHandler` copies a 0x20-byte item record into `D_800A1960`
  using repeated `lwl/lwr` and `swl/swr` pairs:
  offsets `0..0x1F`, copied in chunks such as `0x3/0x0`, `0x7/0x4`,
  `0xB/0x8`, etc.

Why this is hard:

- A plain `struct` assignment only emits this shape if the struct alignment is
  exactly right.
- Too much type information can make GCC use aligned `lw/sw`, which is wrong.
- Too little type information causes byte loops or calls to `memcpy`, also wrong.

What helps:

- Packed/low-alignment blob structs, as used in the room playbook:
  `typedef struct { char b[32]; } Blob32;` with alignment control where possible.
- Verify copy code before decompiling the surrounding state machine.

Severity: high in item/inventory code.

### 7. Large state machines with repeated literal write blocks

Some functions are blocked less by unknown semantics and more by sheer target
shape.

Observed cases:

- `Menu_MainUpdate` is `0x2194` bytes and has a `0x328` stack frame.
- It repeatedly writes bytes into draw-buffer slots derived from
  `D_8009CDDC * 0x48`, for example many blocks of:
  load `D_8009CDDC`, compute `slot*0x48`, store constants at
  `D_800B0158`, `D_800B0159`, `D_800B015A`, `D_800B0160`, etc.
- It also has a non-indexed block that writes directly to `D_800B0158`,
  `D_800B0159`, `D_800B015A`, `D_800B0160`, `D_800B0161`, `D_800B0162`,
  and multiple later rows.

Why this is hard:

- A natural C loop over a table would not match; target is mostly unrolled.
- A natural struct pointer can CSE the active draw slot, while target reloads it
  repeatedly.
- Writing this as literal C is possible but very large and fragile.

What helps:

- First carve out named structs/constants for the draw row format.
- Consider decompiling smaller repeated writer functions elsewhere first, then
  return to `Menu_MainUpdate`.
- Use volatile reads only when target really reloads a global each time.

Severity: very high. This is probably the largest single menu blocker.

### 8. Control-flow lowering for small switches and special cases

GCC 2.7.2 does not always lower C `switch` the way we intuitively expect.

Observed cases:

- `Menu_ItemListInputHandler` has a hand-shaped tree for cases `0`, `1`, `2`
  around stat offsets `0xE`, `0x10`, `0x12`.
- `Menu_CheckItemAffordable` uses several special cases:
  item type `5`, `6`, `0x13`, ranges `6..8`, `0xA`, battle escape flag,
  active weapon category ammo base, and BP/HP checks.
- Division by constants appears as magic multiply in some places, but real
  `div` with break checks appears in `Menu_MainUpdate`.

Why this is hard:

- A C `switch` may become a jump table or a different branch tree.
- Reordering cases for readability often breaks byte-match.
- Signedness controls whether GCC emits `slt`, `sltiu`, `blez`, or `bgtz`.

What helps:

- Rebuild small switches as explicit `if`/`else` trees matching target order.
- Preserve case body order from asm, not from semantic priority.
- Check whether target uses signed or unsigned comparison before naming types.

Severity: medium. Usually solvable once isolated.

### 9. Function-pointer and callback identity

Menu setup functions store draw/input callbacks directly into widget nodes.

Observed cases:

- `Menu_CreateNotificationDialog` stores:
  `Menu_DrawNotificationText` at parent `+0x30`,
  `Menu_HandleDeferredCallbackInput` at parent `+0x2C`,
  `Menu_DrawNotificationDialogContent` at child `+0x30`.
- Existing C often uses `extern M2C_UNK Name[]; #define Name (Name[0])` or
  direct M2C macros to force function-address materialization.

Why this is hard:

- The target may materialize function addresses with `lui/addiu` rather than
  using a direct relocation in a store sequence shaped by C.
- Storing through a typed function pointer can alter register choices or
  warnings, but opaque `void (*)()` loses useful structure.

What helps:

- Prefer typed callback fields in `MenuWidgetNode` when they match existing
  stores.
- Compare address materialization in objdump before polishing semantics.

Severity: medium.

### 10. Existing non-plain-C debt in surrounding menu code

Even when a target function is clean, surrounding C files contain constructs we
do not want to reproduce.

Examples found in current tree:

- `INCLUDE_ASM` still exists in menu files such as `Menu_DrawTextboxEntries.c`,
  `menu32.c`, and `misc24.c`.
- Some menu/main files use `M2C_FIELD`, `M2C_UNK`, and explicit register asm.
- `Menu_AlignEquipPanels.c` contains inline asm barriers.
- `Menu_ExitBonusPointAllocation.c` has symbol alias asm for
  `func_800490B0 = Menu_ExitBonusPointAllocation`.

Why this matters:

- These files often encode exactly the tricks that matched old functions, but
  they can hide the real type/layout structure if copied blindly.
- They can hide the real type structure because many fields are accessed through
  raw offsets.

What helps:

- Treat old M2C files as evidence, not as style to copy.
- Gradually replace shared raw-offset knowledge with headers.
- Do not copy old asm/register levers into new work. If one is used during
  diagnosis, keep it out of the promoted patch and record the target role it
  exposed.

Severity: medium. It slows understanding more than it blocks isolated matches.

### 11. Text buffers and `0xFF`-terminated strings

Menu text is not normal C strings.

Observed cases:

- `Save_InitMetadataState` and `Save_LoadMetadataWindowText` fill windows with
  `0xFF` and copy until `0xFF`, not `0`.
- Notification dialogs use `Util_CopyFFTerminatedBytes` and
  `Util_AppendFFTerminatedBytes`, then measure width with
  `Draw_MeasureTextWidth`.

Why this is hard:

- Natural `strcpy`/`strlen` style C is wrong.
- The first copied byte is often handled outside the loop, because target checks
  whether that first byte is already `0xFF`.
- Signed `char` can break comparisons; use explicit `u8`.

Severity: medium.

### 12. Verification/toolchain friction

This is not a decompilation logic blocker, but it affects confidence and batch
workflow.

Observed case:

- Full `make check` currently reaches `src/main/cdrom/cd.c` and fails with:
  `mipsel-none-elf-as: unrecognized option '--store-return-delay'`.
- This failure is unrelated to the new menu C functions, but it prevents using
  `make check` as a clean end-to-end gate in the current environment.

What helps:

- Continue building touched objects directly.
- Use `make split` after YAML changes.
- Run `make check` anyway and record where it fails.
- Fix the assembler/tool wrapper separately if we want a reliable full gate.

Severity: medium operationally, low for understanding menu code.

## Notes From `/Users/hasik/Projects/ps-decomp-ai`

The handbook is now split into focused shards under
`/Users/hasik/Projects/ps-decomp-ai/docs/ps1_decomp_guide/`. This is more useful
than the original monolithic note because each blocker class has a specific
recipe file and evidence examples.

The older policy in `11_parasite_eve_menu.md` allowed small `register ... asm`,
empty asm barriers, and inline asm helpers as local levers. That is now obsolete
for this project. Those tricks can still be useful as diagnostics to identify a
register-allocation, scheduler, or copy-shape blocker, but promoted
decompilations must match as plain C.

### Which guide shard to open first

| Local blocker | Guide shard |
| --- | --- |
| wrong `sN` allocation, swapped args, delay-slot fence | `07_register_matching.md` |
| switch tree, shared exits, loop rotation, signedness | `09_control_flow.md` |
| widget callbacks, hardware-ish globals, struct fields | `10_hardware_callbacks_structs.md` and `11_parasite_eve_menu.md` |
| current menu function attack notes | `15_parasite_menu_blocker_inventory.md` |
| packed flags, byte masks, action-state bits | `16_packed_flags_bitfields.md` |
| stack frame size, fake locals, spill slots | `18_stack_frame_fake_locals.md` |
| bulk copies, `lwl/lwr`, struct return/prototype ABI | `19_struct_copy_passing.md` |
| rewrite is behavior-correct but not byte-matched | `20_behavioral_rewrite_vs_byte_match.md` |

This gives a better workflow: classify the first mismatch, open the matching
shard, then try only the relevant family of C/source-shape changes. If a
diagnostic asm/register experiment explains the mismatch, translate that
learning back into source structure before promoting anything.

### Techniques worth importing first

- Keep a target register-role table before changing source. For allocator
  blockers, record stack frame size, saved registers, first use, last use, and
  whether each value crosses a call.
- Vary source lifetime deliberately:
  move declarations close to first use, split scopes, assign near use, and keep
  only truly target-reused conditions in locals.
- Compare source shapes before adding any artificial construct:
  `if`/`goto` with shared tail, early returns, `while` versus `do while`, index
  access versus pointer walk, and pre-increment versus post-increment.
- Treat delay-slot differences as source-order evidence. If the target places a
  condition calculation in a delay slot, try making that calculated value common
  to both outgoing paths.
- Treat `__asm__("symbol")`, `M2C_FIELD`, and old asm barriers in existing code
  as evidence of symbol identity, field width, or allocator pressure, not as a
  style to reproduce in new accepted code.
- For globals, decide scalar versus array versus struct block from target
  addressing first. If target uses absolute block addressing, prefer incomplete
  arrays or a block struct over small-data scalar externs.
- For callbacks, establish call graph and callback prototypes before matching
  widget setup. Wrong function-pointer types can change argument registers and
  address materialization.
- For packed runtime state, avoid C bitfields as the first model. If target uses
  `lbu`/`andi`/`ori`/`sb`, start with raw `u8` flags plus explicit masks in the
  same order as target.
- For frame mismatches, stabilize the target frame before fighting registers:
  record `sp` offsets, identify helper-addressed stack slots, and test small
  local structs/arrays or fake scratch slots only where target evidence needs
  them.
- For struct and raw-record movement, decide whether the target is semantic
  field access, aligned bulk copy, or unaligned raw copy before naming fields.
  Field-by-field C is often the wrong first model for menu item records.

### Diagnostic-only asm/register experiments

- Local register binding such as `register T x asm("$s0")` can prove that a
  specific variable-to-register role is the blocker.
- Empty asm barriers can prove that a mismatch is a lifetime or delay-slot
  scheduling problem.
- Inline asm helpers for `lwl/lwr/swl/swr` can prove that an unaligned-copy
  primitive is the blocker.

These experiments should stay in scratch work. The accepted patch must remove
them and express the same shape in plain C. Scratch notes should record the
closest pure-C attempt: size, first mismatch, register allocation, and why the
remaining mismatch points to a specific source-shape problem.

### Still disallowed

- Whole-function asm bodies or `.word` bodies.
- Maspsx output hacks or postpass rewriting.
- Build-system hacks that hide a nonmatching function.

### Decomp.me examples to mine without copying code

The handbook's feature index gives useful target-asm oracles:

| Slug | Why it is relevant |
| --- | --- |
| `opdGv` | long repeated `lwl/lwr/swl/swr` raw-record copies; useful for item-record copy shape |
| `nokIM` | `lwl/lwr` copy from argument pointer to stack; useful for low-alignment stack temporaries |
| `MQVsQ`, `OIgwc`, `9DUZU`, `Og432`, `AfP8N` | additional unaligned-copy / decompiler-failure cases |
| `TB9Jm`, `KRack` | examples where lifetime fences solved scheduling; useful as a model for minimal barriers |
| `YX4pg` | callback plus irregular switch; useful for widget/update callback setup |
| `HnKWT`, `rbbT9`, `ngeVN` | `.sdata`/`.sbss` and `$gp` classification cases |

Use these as shape references: inspect target instruction order, register roles,
and copy offsets, then try ordinary C first. If the remaining mismatch is a
single allocator/scheduler/copy primitive issue, record that diagnosis and keep
iterating on plain-C structure rather than keeping the whole function in asm or
adding a local asm lever.

### Updated plain-C recipes for our current blockers

`Menu_InventoryPageInputHandler`:

- Start from the exact-size `0x128` C attempt, not from scratch.
- Preserve the target role table:
  `s4=root`, `s2=flags`, `s3=handled`, `s0=child`, `s1=page node`.
- Try source variants where `flags & 0x1000` is computed once in a path that
  dominates both handled and unhandled flow. The target's delay slot suggests
  that this value is shared.
- Move `child` and `node` declarations closer to their first real use if they
  steal lower `sN` registers from `flags`/`handled`.
- Prefer one shared exit label while iterating, because multiple `return` paths
  change delay-slot fill and live ranges.

`Save_InitMetadataState`:

- Keep the large globals as arrays/incomplete objects where target addressing is
  absolute, not gp-relative.
- Duplicate the two text-window load blocks in source. A helper call is cleaner
  but does not describe the target.
- Split source cursor and destination cursor scopes aggressively so GCC has less
  reason to preserve an extra cursor in `s1`.
- Test first-byte `0xFF` handling outside the copy loop; the handbook shows this
  is common in PS1 text buffers.

`Menu_ItemListInputHandler`:

- Isolate the 0x20-byte copy into a scratch before decompiling the whole state
  machine.
- Write down the exact target pairs first:
  `src+3/src+0 -> dst+3/dst+0`,
  `src+7/src+4 -> dst+7/dst+4`,
  continuing through `0x1F/0x1C`.
- Try low-alignment blob assignments and `char*`/packed-wrapper variants. Do
  not use `memcpy` unless the target calls it.
- If those cannot emit the target `lwl/lwr/swl/swr` sequence, record the closest
  C copy attempt and keep searching for a plain-C representation; do not promote
  an inline asm copy primitive.
- Once the copy is solved, return to the state machine and preserve the target
  if-tree order for cases around item offsets `0xE`, `0x10`, and `0x12`.

`Menu_MainUpdate`:

- Open `16_packed_flags_bitfields.md` before modeling action/menu flags. Use raw
  byte/halfword storage and explicit masks; do not introduce C bitfields early.
- Open `18_stack_frame_fake_locals.md` before matching the `0x328` frame. Record
  which stack slots are real locals, helper-addressed scratch, and spills.
- Keep a region notebook separate from match status: intro/fade, pad gating,
  draw-buffer writes, actor/menu sync, teardown. Mark each region `understood`,
  `near-match`, or `matched` using the status language from
  `20_behavioral_rewrite_vs_byte_match.md`.

`Menu_CreateNotificationDialog`:

- Confirm callback field prototypes before matching stores:
  parent input callback at `+0x2C`, parent draw callback at `+0x30`, child draw
  callback at `+0x30`.
- Compare direct field assignment versus temporary function-pointer locals. The
  target address materialization order matters more than initializer aesthetics.

`Menu_DrawSelectableEquipSlotRow`:

- This is now a good candidate for a tiny register binding if source-shape
  variants still cannot reproduce the item/dimmed register roles. Keep the row
  draw logic in C and bind only the necessary locals.

## What Actually Blocks Us Most

Ordered by current impact:

1. Callee-save register allocation mismatch after semantic match.
   This blocks small functions that should otherwise be easy.
2. Large state machines with repeated global reload/write patterns.
   This blocks `Menu_MainUpdate` and much of item-list input.
3. Incomplete runtime structure knowledge.
   This affects every large menu function and makes C too raw-offset-heavy.
4. Unaligned item-record copies.
   This is a concrete blocker inside `Menu_ItemListInputHandler`.
5. Small-data versus absolute-address classification.
   Often solvable, but it can cause misleading size/register diffs.
6. Inlined helper duplication.
   Especially visible in save metadata init.
7. Existing M2C/asm debt.
   Useful as evidence, dangerous as implementation style.

## Recommended Attack Plan

### Short term

- Keep converting functions where register allocation is not fighting us:
  draw/setup functions with narrow live ranges.
- For each parked function, record:
  target `sN` roles, exact C size reached, and first instruction mismatch.
- Build a small header for confirmed menu/BP/save structs instead of scattering
  one-off typedefs.

### Medium term

- Decompile helper-sized save functions first:
  `Save_GetActiveMetadataBuffer`, `Save_LoadMetadataWindowText`,
  `Save_AppendMetadataChar`, `Save_DeleteMetadataChar`, `Save_RestoreHeader`.
  They directly explain `Save_InitMetadataState`.
- Isolate the item-record copy shape from `Menu_ItemListInputHandler` into a
  tiny scratch experiment until we know which C struct emits the target
  `lwl/lwr` sequence.
- Name the item/action globals around `D_8009CF0C..D_8009CFD4`.

### Long term

- Treat `Menu_MainUpdate` as multiple regions, not one monolith:
  intro/fade update, pad/action-gating, draw-buffer row writes, actor/menu-state
  synchronization, teardown/close paths.
- Before full C, document each region's globals and write patterns. A direct
  one-shot rewrite is likely to produce an enormous nonmatch that is hard to
  debug.

## Parked Function Notes

### `Menu_InventoryPageInputHandler`

Status: semantically reconstructed; object reached exact size `0x128`.

Blocker: wrong callee-save allocation. Target wants:

- `s4 = root`
- `s2 = flags`
- `s3 = handled`
- `s0 = child`
- `s1 = page node`

Plain C kept assigning child/node/flags/handled differently. A one-variable
register binding proved the blocker in earlier scratch work, but it is not
acceptable for promotion under the current plain-C rule.

### `Save_InitMetadataState`

Status: scratch C got close:

- first version: `0x224`
- after fixing large-global declarations: `0x23C`
- target: `0x24C`

Known improvements:

- Large globals must be arrays/incomplete objects to avoid gp-relative stores.
- The two text-window loads are inlined copies of `Save_LoadMetadataWindowText`.

Remaining blocker:

- GCC keeps an extra `s1` cursor and a larger prologue/epilogue; target uses
  only `s0` as callee-save.

### `Menu_DrawSelectableEquipSlotRow`

Status: previous attempt only matched with explicit register binding.

Blocker: target register allocation for item/dimmed locals. Do not reintroduce
`register ... asm("$sN")` in accepted code; use the previous result only to
guide source-shape experiments.

### `Menu_FindSelectedEquipSlotItem`

Status: useful for understanding equip-slot tables, but parked.

Blocker: stack frame/reload ordering. It appears small, but the target's reload
timing does not fall out of straightforward C.

### `Menu_DrawAmmoTypeHeader`

Status: useful for widget layout.

Blocker: C reached plausible size/logic but did not fully match. It confirmed
that `+0x58` is `y_limit`, not `scroll_y`.

## Practical Checklist Before Trying a Hard Function

1. Write down target stack frame size and saved registers.
2. Write down each `sN` role before writing C.
3. Identify gp-relative globals versus absolute-address globals from objdump and
   `build/USA/main.map`.
4. Decide whether data should be a scalar, array, or struct block before the
   first compile.
5. Check for `lwl/lwr`, magic multiply, real `div`, and jump-table-like control.
6. If a semantic C version is close but wrong, record:
   exact size, first mismatch, register allocation, and whether the mismatch is
   source-shape, type/layout, scheduler, or unaligned-copy related.
