# Functions not credited as matched C (audit 2026-10-04)

Source: `make report` (build/USA/report.json) on origin/main c687b258c minus what this
audit branch matched (see the last section); every function whose objdiff
match is below 100% is listed (124 entries before this branch).
Size is the objdiff function size in bytes.

## Summary

| Category | Functions | Bytes |
|---|---:|---:|
| battle (excluded) | 13 | 26172 |
| handwritten library/BIOS asm | 28 | 3116 |
| data/pad/slice | 36 | 13156 |
| needs-goto / stack switch | 6 | 6128 |
| inline-asm C unit | 0 | 0 |
| parked near-miss | 28 | 48840 |
| not yet attempted | 0 | 0 |
| total | 111 | 97412 |

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
| main | Battle_DrawStatusPanel | 2128 |  |
| main | Battle_UpdateEnemy | 2144 |  |
| main | Battle_PhaseHitReaction | 2664 |  |
| main | Battle_StepAyaAction | 2688 |  |
| main | Battle_DrawHPBar | 3152 |  |

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
| main | Render_SetupColorTable | 644 | candidates/main/render/Render_SetupColorTable: lev 87 (direct digit lvalues give retail digit base; -1 hoist and register numbering left) |
| main | func_800C2758 | 920 | candidates/main/engine/func_800C2758.c (old byte-offset draft, no README, diff count not recorded); OWNED BY ANOTHER AGENT |
| main | func_800D3BC8 | 924 | candidates/main/engine/engine_800D3BC8: lev 14 (prologue save placement only) |
| main | Akao_EnqueueStagedCommand | 968 | candidates/main/main/Akao_EnqueueStagedCommand/struct_staging.c: lev 16 (scratch struct view; needs staging struct + word-opcode queue entry in shared headers, see README) |
| main | func_800CEE20 | 1420 | candidates/main/engine/engine_800CEE20: 6 diffs |
| main | Entity_FrameUpdate | 1836 | candidates/main/entity/Entity_FrameUpdate.c (old byte-offset draft, no README, diff count not recorded); OWNED BY ANOTHER AGENT |
| main | func_800D0728 | 1888 | candidates/main/engine/engine_800D0728: 4 extra instructions |
| main | Entity_UpdateAndRender | 1980 | candidates/main/entity/Entity_UpdateAndRender_typed: -G8/-G4 typed draft at lev 172 (spills/regalloc in the ramp edge test, still gotos), see README; OWNED BY ANOTHER AGENT |
| main | Render_SetupEntityPrims | 2012 | candidates/main/main/Render_SetupEntityPrims_typed: lev 4, texture loops solved (shared `src` cursor), only the paletteRow/initCount a2/t9 global-alloc order is left, see README |
| main | func_800CAE0C | 2372 | candidates/main/engine/engine_800CAE0C: lev 50 (needs one more counted reference to rotation + 2 in layer 4, see README) |
| main | Geo_ClipToFloorBoundary | 2920 | candidates/main/main/Geo_ClipToFloorBoundary_typed: typed rewrite at lev 413 (frame 208 vs 192: an extra reduced walk pointer per half), see README |
| main | MemCard_UpdateSaveState | 3864 | candidates/main/memcard/tu_031908.c (old byte-offset draft, no README, diff count not recorded) |
| fx_common | RoomLib_HandlerD | 748 | one 0x3D0 path sampler (report splits it at a stale RoomLib_HandlerD symbol at 0x8018F640); candidates/overlays/fx_common_sample_path: lev 17 without volatile (count read gives lh, not lhu/sll/sra) |
| fx_common | func_80193B5C | 1452 | candidates/overlays/fx_common_effect_markers: ~500 diffs (register allocation) |
| menu_memcard | func_801EDC44 | 2384 | candidates/overlays/menu_memcard_func_801EDC44: 34 diffs plain, 2 diffs with lift shared with the mode 1 vz (retail lift is a global pseudo; vz register still differs) |
| room_m256 | func_80195728 | 1440 | candidates/overlays/room_m256_func_80195728: lev 43 |
| scene_e08 | func_80191E78 | 848 | candidates/overlays/scene_e08_func_80191E78: lev 16 (kind = 4 for parameter02; first block 0x40 placement left) |
| scene_e20 | func_8018F028 | 1832 | candidates/overlays/scene_e20_func_8018F028: 7 diffs (single `special` variant, 2026-10-04) |

## not yet attempted

None: menu_memcard 0x244C now has a parked candidate.

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
