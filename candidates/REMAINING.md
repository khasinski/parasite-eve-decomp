# Functions not credited as matched C (audit 2026-10-04)

Source: `make report` (build/USA/report.json) on origin/main c687b258c minus what this
audit branch matched (see the last section); every function whose objdiff
match is below 100% is listed (124 entries before this branch).
Size is the objdiff function size in bytes.

## Summary

| Category | Functions | Bytes |
|---|---:|---:|
| battle (excluded) | 8 | 13396 |
| handwritten library/BIOS asm | 28 | 3116 |
| data/pad/slice | 36 | 13156 |
| needs-goto / stack switch | 6 | 6128 |
| inline-asm C unit | 0 | 0 |
| parked near-miss | 20 | 33312 |
| not yet attempted | 0 | 0 |
| total | 98 | 69108 |

Category notes:
- **inline-asm C unit**: the C file contains CPU instruction asm, so the whole
  unit is classed asm_constrained and none of its functions is credited, even
  the plain-C siblings.
- **data/pad/slice**: not code, or not a compilable function on its own.

## battle (excluded)

| Binary | Function | Size | Notes |
|---|---|---:|---|
| main | Battle_StepPlayerTurn | 1492 |  |
| main | Battle_BuildStatusPrimHeader | 1548 |  |
| main | Battle_StepVictory | 1616 |  |
| main | Battle_StepPostBattle | 1712 |  |
| main | Battle_DrawEnemyHP | 1712 |  |
| main | Battle_DrawActiveStatus | 1732 |  |
| main | Battle_StepEnemyMovement | 1748 |  |
| main | Battle_ResolveHitOnTimer | 1836 |  |

## handwritten library/BIOS asm

| Binary | Function | Size | Notes |
|---|---|---:|---|
| main | Math_FixedRoundToInt | 16 | game-side handwritten helper (`add`/`sub` with $at, `or ra` save; spimdisasm flags it); C file reproduces it with inline asm |
| main | Math_FixedRoundToByte | 16 | game-side handwritten helper (`add`/`sub` with $at, `or ra` save; spimdisasm flags it); C file reproduces it with inline asm |
| main | func_8007E3C8 | 16 | PSY-Q LIBCARD PATCH/END (assembler source) |
| main | Gte_NormalizeVecS32toS16 | 20 | PSY-Q LIBGTE (assembler source) |
| main | Math_FixedMul | 28 | game-side handwritten helper (`add`/`sub` with $at, `or ra` save; spimdisasm flags it); C file reproduces it with inline asm |
| main | func_8007E344 | 44 | PSY-Q LIBCARD PATCH/END (assembler source) |
| main | Gte_NormalizeVec | 48 | PSY-Q LIBGTE (assembler source) |
| main | VectorNormalSS | 48 | PSY-Q LIBGTE (assembler source) |
| main | Task_GpuPackPrimColor | 52 | game-side handwritten helper (`add`/`sub` with $at, `or ra` save; spimdisasm flags it); C file reproduces it with inline asm |
| main | __do_global_dtors | 104 | PSY-Q LIBSN SNMAIN.OBJ (assembler source) |
| main | Pad_StopHandler | 104 | PSY-Q LIBAPI PATCH/CHCLRPAD (assembler source) |
| main | __main | 112 | PSY-Q LIBSN SNMAIN.OBJ (assembler source) |
| main | Pad_DequeueHandler | 112 | PSY-Q LIBAPI PATCH/CHCLRPAD (assembler source) |
| main | func_8007E470 | 112 | PSY-Q LIBCARD PATCH/END (assembler source) |
| main | _ExitCard | 112 | PSY-Q LIBCARD PATCH/END (assembler source) |
| main | InitGeom | 128 | PSY-Q LIBGTE (assembler source) |
| main | Gte_ISqrt | 132 | PSY-Q LIBGTE (assembler source) |
| main | Math_SqrtApprox3 | 136 | game-side handwritten helper (`add`/`sub` with $at, `or ra` save; spimdisasm flags it); C file reproduces it with inline asm |
| main | Gte_VectorOp | 140 | PSY-Q LIBGTE (assembler source) |
| main | func_8007E3DC | 148 | PSY-Q LIBCARD PATCH/END (assembler source) |
| main | Gte_PushMatrix | 164 | PSY-Q LIBGTE (assembler source) |
| main | Gte_PopMatrix | 164 | PSY-Q LIBGTE (assembler source) |
| main | __SN_ENTRY_POINT | 168 | PSY-Q LIBSN SNMAIN.OBJ (assembler source) |
| main | St_InstallDmaHandler | 172 | PSY-Q PATCHGTE exception patcher (assembler source) |
| main | Gte_MatrixOp | 192 | PSY-Q LIBGTE (assembler source) |
| main | Gte_BuildOrthoBasis | 232 | PSY-Q LIBGTE (assembler source) |
| main | CompMatrix | 352 | PSY-Q LIBGTE (assembler source) |
| sys_reset | func_8010C4CC | 44 | sys_reset handwritten helper (`addi $at`) |

## data/pad/slice

| Binary | Function | Size | Notes |
|---|---|---:|---|
| main | func_8003E60C | 4 | zero padding after `jr ra` (single `nop` labelled as a function) |
| main | func_80070E04 | 4 | zero padding after `jr ra` (single `nop` labelled as a function) |
| main | func_80081310 | 4 | zero padding after `jr ra` (single `nop` labelled as a function) |
| main | func_800835A0 | 4 | zero padding after `jr ra` (single `nop` labelled as a function) |
| main | St_DmaCompleteCallback | 20 | instruction templates copied as data (text_data unit) |
| main | RawData_80074354 | 24 | raw words, invalid instruction |
| main | D_8007A1F8 | 24 | instruction templates copied as data (text_data unit) |
| boot_display | D_80125B86 | 8 | dlabel data inside a code segment |
| boot_display | D_80125B1E | 104 | dlabel data inside a code segment |
| boot_display | D_80125B8E | 114 | dlabel data inside a code segment |
| boot_display | D_80125A48 | 214 | dlabel data inside a code segment |
| fx_field | func_8018EFE8 | 28 | fx_field header/data words (text_data unit) |
| fx_field | func_8018FC78 | 880 | fx_field header/data words (text_data unit) |
| menu_memcard | D_801223F4 | 1 | dlabel data inside a code segment |
| menu_memcard | D_801223F5 | 1 | dlabel data inside a code segment |
| menu_memcard | D_801228E2 | 4 | dlabel data inside a code segment |
| menu_memcard | D_801228E6 | 4 | dlabel data inside a code segment |
| menu_memcard | D_801228F2 | 4 | dlabel data inside a code segment |
| menu_memcard | D_801228F6 | 4 | dlabel data inside a code segment |
| menu_memcard | D_801228EA | 8 | dlabel data inside a code segment |
| menu_memcard | D_80120D00 | 772 | dlabel data inside a code segment |
| menu_memcard | D_801228FA | 1126 | dlabel data inside a code segment |
| menu_memcard | D_801223F6 | 1260 | dlabel data inside a code segment |
| menu_memcard | D_8012B7A8 | 1436 | dlabel data inside a code segment |
| menu_memcard | D_8018EB90 | 1892 | dlabel data inside a code segment |
| render_clip | func_80170000 | 944 | render_clip.bin is a byte copy of fx_common 0x8800-0x9000: mid-function slice of FxCommon_EffectInitializationFlow plus a truncated FxCommon_DrawPolyResource (both C in fx_common) |
| render_clip | func_801703B0 | 1104 | render_clip.bin is a byte copy of fx_common 0x8800-0x9000: mid-function slice of FxCommon_EffectInitializationFlow plus a truncated FxCommon_DrawPolyResource (both C in fx_common) |
| scene_e14 | D_8018EFE8 | 36 | dlabel data inside a code segment |
| scene_e19_2 | func_8018EFF0 | 1204 | scene_e19_2 scene_pre: starts mid-function (no prologue, stores to 0x238(sp)); tail slice of a function from another overlay |
| scene_e22 | D_801994EE | 8 | dlabel data inside a code segment |
| scene_e22 | D_801990C0 | 250 | dlabel data inside a code segment |
| scene_e22 | D_801994F6 | 754 | dlabel data inside a code segment |
| scene_e22 | D_801991BA | 820 | dlabel data inside a code segment |
| sys_reset | func_8010BE30 | 4 | zero padding after `jr ra` (single `nop` labelled as a function) |
| sys_reset | func_8010C4C4 | 4 | zero padding after `jr ra` (single `nop` labelled as a function) |
| sys_reset | D_8010BCF8 | 84 | dlabel data inside a code segment |

## needs-goto / stack switch

| Binary | Function | Size | Notes |
|---|---|---:|---|
| main | main | 748 | Boot_MainLoop: goto dispatch plus scratchpad stack switch around func_8019234C (inline asm in C unit) |

## inline-asm C unit

None left: CdRom_InitDsCallbacks and func_800C2D0C are plain C on this branch.

## parked near-miss

| Binary | Function | Size | Notes |
|---|---|---:|---|
| main | Render_SetupColorTable | 644 | candidates/main/render/Render_SetupColorTable: lev 87 (direct digit lvalues give retail digit base; the unhoisted -1 occupies a setup temp and pushes style out of a1; then values keeps the a2 preference, see README) |
| main | func_800C2758 | 920 | candidates/main/engine/func_800C2758.c (old byte-offset draft, no README, diff count not recorded); OWNED BY ANOTHER AGENT |
| main | func_800D3BC8 | 924 | candidates/main/engine/engine_800D3BC8: lev 14, prologue only (saves scheduled into load stalls; retail shape not reachable under stock sched2 rules, see README) |
| main | func_800CEE20 | 1420 | candidates/main/engine/engine_800CEE20: 6 diffs |
| main | Entity_FrameUpdate | 1836 | candidates/main/entity/Entity_FrameUpdate.c (old byte-offset draft, no README, diff count not recorded); OWNED BY ANOTHER AGENT |
| main | func_800D0728 | 1888 | candidates/main/engine/engine_800D0728: 4 extra instructions |
| main | Draw_AllocTexturedRectAlt | 2584 | candidates/main/main/Draw_AllocTexturedRectAlt_typed: lev 275, first typed pass (slice pointer copy into a3 merged by cse), see README; OWNED BY ANOTHER AGENT |
| main | MemCard_UpdateSaveState | 3864 | candidates/main/memcard/tu_031908.c (old byte-offset draft, no README, diff count not recorded) |
| fx_common | RoomLib_HandlerD | 748 | one 0x3D0 path sampler (report splits it at a stale RoomLib_HandlerD symbol at 0x8018F640); candidates/overlays/fx_common_sample_path: lev 17 without volatile (count read gives lh, not lhu/sll/sra) |
| menu_memcard | func_801909B4 | 3908 | candidates/overlays/menu_memcard_func_801909B4 (no README, diff count not recorded) |
| scene_e08 | func_80191E78 | 848 | candidates/overlays/scene_e08_func_80191E78: lev 15 with shared load temporaries feeding the parameter block (steering-grade, see README round 6); lev 16 with the plain `kind = 4` form |
| scene_e20 | func_8018F028 | 1832 | candidates/overlays/scene_e20_func_8018F028: 7 diffs (single `special` variant, 2026-10-04) |

## not yet attempted

None.

## Matched on this audit branch

| Binary | Function | Size | How |
|---|---|---:|---|
| scene_e09 | func_8018F420 | 824 | RoomLib_InitMotionParticles template (room_m075) with renamed tables |
| scene_e10 | func_8018F420 | 824 | same |
| main | Math_FixedDivide | 20 | split out of the asm_constrained math_fixed unit |
| main | Gpu_InitDrawModeSprtPacket | 100 | split out, typed packet record |
| main | Inv_BuildItemGridFromCategory | 452 | inline mult asm was GCC's own i %% 3; indexed record/column form |
| main | Util_CopyFFTerminatedBytes, Util_AppendFFTerminatedBytes, Inv_SelectActiveList | 288 | credited once util.c became plain C |
| main | Akao_StepSampleLoader | 2048 | typed note step (lev 8 to 0): the drum volume product goes through `value = value * sum; expression_value = value << 2;` (output reload from lo into the volume register, mflo a1), and the pitch LFO depth is `lfo_depth = depth * x >> 7` in both branches with one store after the join, so jump2 cross-jumps the mflo/srl tails and the store stays in the join block next to the selector load; restart stores are table, counter, phase. Bit 0x200000 is AKAO_TRACK_FLAG_KEY_OFF_PENDING (set instead of key-off when AKAO_TRACK_FLAG_SUSTAIN 0x100000 is on) |
| main | CdRom_InitDsCallbacks | 152 | plain C under the `ASSEMBLER: GNU` marker its LIBDS siblings use: GNU as in reorder mode moves the `sw` of `g_DsPollCallback = 0` into the CdRom_InitCmdState delay slot |

The menu_memcard video step pair (func_80122040 at 0x1340, func_8012AE88 at 0xA144) is now matched on main by another agent and is left out of the table.
| main | func_800C2D0C | 148 | plain C (stock 2.7.2, maspsx): the 8-byte frame comes from the `s16 offset` local, sched2 sinks the prologue `addiu sp` to the branch and the assembler fills the delay slot with it; `offset += size; state->data_next = offset;` gives the in-place add |
| main | Entity_UpdateAndRender | 1980 | lev 0 under -G8 cc1 / -G4 maspsx with typed collision records: the packed sxy words go through one shared `int sxy` (set twice, so global alloc gives the or result v1), `a = abs(a)` (abssi2) for the area magnitude, the area test as one condition with nested box tests (no leave gotos), the clip results kept in `i` so the third loop entry compares it, and a static inline revert helper at each rollback site (jump2 cross-jumps them); 1 goto left (slide entry), logged as debt |
| main | Geo_ClipToFloorBoundary | 2920 | lev 0 under -G8 cc1 / -G4 --expand-div maspsx, no gotos: the edge walk indexes the triangle by slot (loop.c reduces it to one walk pointer plus the slot*2 byte offset and rewrites the exit test against base + 6), the visited-edge pointer, prevIndex/prevX/prevZ are function-scope so they are set in both halves (multi-set: no birthing boost in sched1, and the doubled refs give prevX s6), one shared `int d` for the box limits, distance, projection and squared distances with the divisor loaded into it, `kind` byte view for the neighbour flag, tentative COMMON declarations of D_8009CE0C/D_8009CE18 so maspsx keeps the load-delay nop before the gp stores, `continue` with `while (++slot < 3)` instead of the skip gotos |
| main | Render_SetupEntityPrims | 2012 | lev 0 (plain -G0): the last a2/t9 swap of paletteRow/initCount was a global-alloc priority tie; writing the matrix command rounding step by step on the shared `bytes`/`words` temporaries (`words = (u16)bytes; words >>= 2;`, `bytes = words; bytes++; bytes *= 4;`) adds three insns that combine later merges, so both live lengths grow by 3 and the tie goes to paletteRow, as in retail |
| fx_common | func_80193B5C | 1452 | FxCommon_DrawEffectMarkers, lev 0: mode and OT links written as 24-bit bitfield copies (`mode->tag.bits.address = allocation[10].bits.address`, the extract and insert masks give the prologue mask its 4th reference, so global alloc puts it in a3 and level in t0), RotTransPers3-style scalar `s32` outputs instead of a struct (the label's screen read is no longer in-struct, so it does not depend on the line link stores and the xy stores drop to the colour/uv priority), then setXY4/setUV4 field order in both labels |
| menu_memcard | func_801EDC44 | 2384 | Memcard_RingBurstController, lev 0: one function-scope `amount` holds both the state 1 ring fade (`amount = 0x80 - burst->timer * 32;` passed to func_800D1AE0 inside the `timer < 5` block) and the band lift (`func_80077CF4(angle) / 12 + 80`); combine folds the fade copy into a1, but flow already counted it in another basic block, so the lift is a global pseudo and global alloc gives retail's whole map (lift s4 after the block locals &band s1, &tilt s2, &offset s3), with a block-local `dim = fade * 2 / 3` |
| main | Battle_StepAyaAction | 2688 | score 0 and whole-main byte-match, committed in 66998969c; stock tools, 14 pins and 7 empty barriers recorded in debt |
| main | Battle_PhaseHitReaction | 2664 | score 0 and whole-main byte-match; existing turn-phase palette code, typed enemy floating panel fields at 0xD0..0xD6, 11 color pins, 7 empty barriers and the 0x1A0-byte unused stack reserve recorded in debt |
| main | func_800CAE0C | 2372 | FieldEng_GlowFourLayers: score 0 and whole-main byte-match with stock GCC/MASPSX; one empty barrier after the fourth CompMatrix keeps the column address live and reproduces the retail spill, no pins; existing SDK GTE macros |
| main | Battle_UpdateEnemy | 2144 | score 0 and whole-main byte-match with stock GCC/MASPSX; shared EnemyCombatant charge, motion, saved animation and damage-panel fields; minimized to 10 pins and 3 empty barriers recorded in debt |
| main | Battle_DrawStatusPanel | 2128 | score 0 and whole-main byte-match with stock GCC/MASPSX; shared BattleStatusPanel and RenderSpritePacket layouts; 14 pins, 4 empty barriers, 48-byte unknown stack reserve and matching symbol views recorded in debt |
