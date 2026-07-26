# Global Naming Reference

Canonical source names for globals whose role follows from multiple callers or
from a defining write. Addresses remain the linker symbols through `__asm__`.
Entries below are deliberately limited to confirmed meanings.

| Address | Canonical name | Type / role | Confidence | Evidence |
|---|---|---|---|---|
| `0x8009D20C` | `g_FieldActorListHead` | Head of the linked `FieldActor` / battle-entity list | High | Walked through `next` at `+0x04` by `Entity_GetPositionByType`, `Task_CopyEntityRenderObject`, and `Akao_ProcessCommand`. |
| `0x8009D254` | `g_PlayerEntity` | Active player entity, which is also the current Aya battle entity | High | Used as the type-0 entity in task commands and as the player object in field and battle code. `battle.h` documents the same pointer. |
| `0x8009D2F0` | `g_CurrentEntity` | Entity selected for the current task/script command | High | Task handlers read and mutate its transform, animation, parent, and render fields. The AKAO dispatcher reads its type/subtype. |
| `0x8009CDDC` | `g_ActiveDrawSlot` | Active double-buffer draw slot | High | Toggled with `^= 1` around CLUT/draw-environment updates and already named in render sources. |
| `0x8009D1A0` | `g_GameStateFlags` | Cross-subsystem game-state bitfield | High | Tested and updated by boot, scene, entity, battle, and menu transitions; existing sources already use the name. |
| `0x8009D278` | `g_ActiveActor` | Current `Combatant` selected by the battle state machine | High | Battle dispatch stores the selected entity's `core` pointer here; battle updates read its action and state fields. |
| `0x8009D124` | `g_TextCursorX` | Shared X cursor for text and menu primitive emission | High | Glyph, number, widget, and shadow emitters read, save, restore, and advance it. |
| `0x8009D128` | `g_TextCursorY` | Shared Y cursor for text and menu primitive emission | High | Paired with `g_TextCursorX` in every text/layout stack operation. |
| `0x8009CDB0` | `g_TextGlyphSpacing` | Extra horizontal advance of the current glyph | High | Set from glyph class/metrics and added to the X advance and text-width calculations. |
| `0x800F32D0` | `g_FieldEngineObject` | Object currently installed for field-engine callback processing | High | `func_800D4698` installs `obj`; `func_800D3F64` and `func_800D3FD8` consume it to resolve its child data. |
| `0x800E2368` | `g_FieldEngineContext` | Callback context associated with `g_FieldEngineObject` | High | `func_800D4698` stores `obj + 0xC`; callback dispatchers pass its word at `+0x08`. |
| `0x8009D26C` | `g_FieldInputState` | Normalized field input bitmask | High | `Field_HandleStateTransition` derives it from pad/analog input, applies menu gating, then consumers use its directional and action bits. |
| `0x8009D2E8` | `g_EntityControlFlags` | Cross-mode entity-control flags | Medium | Battle sets/clears action bits; field scene and sprite code use bit `0x10` to mirror the player-relative facing. The individual bit meanings still need names. |
| `0x8009D2C8` | `g_AkaoVoiceState` | Pointer to the active AKAO voice-state bank | High | The queue processor temporarily moves it by `0x68` to process the secondary bank; sample loading and SPU updates consume masks and mixer settings through it. |
| `0x800966EC` | `g_SinCosTable` | 4096-entry signed 16-bit sine/cosine lookup table | High | Field, room, and scene motion index it with angles masked to `0xFFF`; entries are consistently accessed as two `s16` components. |
| `0x800E27EC` | `g_ActiveEffectFrame` | Elapsed frame counter of the currently dispatched field-effect entry | High | `func_800CE688` installs each entry's counter before its mode-1 callback and increments it until completion; `func_800CE78C` installs the matching counter for mode 2, restoring the prior value afterward. |
| `0x8009D1D4` | `g_BattleActionQueueCursor` | Index of the action currently being processed in the battle queue | High | Battle action, hit, phase, and script routines index the 8-byte entries at `D_800BE830` with it and advance/reset it when an entry completes. |
| `0x8009CE3C` | `g_BattleActionQueueCount` | Number of queued battle actions | High | Enemy-turn code appends entries by incrementing it; every queue consumer compares the cursor to it, and queue teardown clears it. |
| `0x800B0CD8` | `g_GameState` | Shared game-state record beginning with global mode flags | High | Core, field, battle, FMV, menu, memory-card, render, and AKAO code use the record and its fields through a common base. It is not only a flag word: offsets beyond `+0x100` hold render primitive state. |
| `0x800F3368` | `g_FieldEffectRenderState` | Nine-word (`s16`) render-state block for the active field effect | Medium | Effect emitters initialize the contiguous range through `0x800F3378` before allocating particles; its fields feed texture selection, scale/offset, and timing arguments. Individual field names remain unconfirmed. |
| `0x800B1624` | `g_GeomState` | Active field/room geometry and GPU draw context | High | Its header holds control-entry and render-entry offsets. Object, scene, camera, and renderer code all resolve their records through this pointer; `geom_state.h` already models the verified layout. |
| `0x800E2248` | `g_FieldEngineScriptState` | State block of the field-engine script currently being interpreted | High | Every interpreter entry point installs `obj + 0xC`; it contains the delay, instruction cursor, stop/status bytes, 16 script variables, and the `+0x70` caller value. |
| `0x800F32A8` | `g_FieldEngineScriptObject` | Owning object of `g_FieldEngineScriptState` | High | Installed together with the state by the interpreter entry points as the original `obj` pointer. |
| `0x800F3330` | `g_FieldEngineScriptData` | Data area of the active field-engine script object | High | Installed as `obj + 0x200`; callback entries use their signed offset relative to this base. |
| `0x800F33B0` | `g_FieldEngineCommandTable` | 32-bit command stream for the active field-engine script | High | Installed from `obj + 0x78` and indexed by the instruction cursor in `func_800C2758`. |
| `0x800F34F4` | `g_FieldEngineSlotTable` | 64-entry, 6-byte field-engine slot table | High | Installed as `obj + 0x80`; registration, allocation, and completion routines index it by `slot * 6`. |
| `0x8009D100` | `g_DrawPacketCursor` | Next free byte in the shared GPU primitive packet arena | High | Every primitive allocator returns the old value, advances it by the primitive size, and bounds-checks the result. |
| `0x8009D104` | `g_DrawPacketArenaBase` | Base of the 16 KiB shared GPU primitive packet arena | High | Allocators reject a packet when `cursor + size >= base + 0x4000`. |
| `0x8009D11C` | `g_DrawOrderingTableEntry` | Active ordering-table entry used to link emitted GPU primitives | High | Primitive emitters splice packet tags through `*g_DrawOrderingTableEntry` before updating it to the new primitive. |
| `0x8009D12C` | `g_TextCursorStack` | Stack cursor for saved text cursor X/Y pairs | High | Text/menu emitters push two `s32` coordinates, advance by two words, and pop the same pair with bounds checks. |
| `0x8009D110` | `g_DrawPrimaryColor` | Default packed RGB color for generated primitives | High | Used whenever the draw color selector is clear; menu and primitive helpers install colors such as `0x808080` and `0x303030`. |
| `0x8009D114` | `g_DrawAlternateColor` | Alternate packed RGB color for generated primitives | High | Selected when `D_8009D10C` is nonzero and consistently paired with `g_DrawPrimaryColor`. |
| `0x800942EC` | `g_RoomRenderYOverride` | Global Y translation used by flagged room/entity render paths | Medium | GPU initialization clears it; room rendering substitutes it for an actor's Y coordinate when flag `0x04000000` is set, and the battle context setter exposes it as selector `255`. It is not a frame counter. |
| `0x800B0E38` | `g_OtBufferTable` | Per-draw-slot ordering-table bases | High | Render, battle HUD, room, and menu code select `g_ActiveDrawSlot` and append primitives at depth-specific offsets; the manual linker symbol already uses this name. |
| `0x800B0E64` | `g_LoadedSceneAssetBlock` | Base pointer of the currently loaded scene/room asset block | High | Room and scene overlays pass it into their standard effect/asset constructors, while the AKAO dispatcher uses its offset tables. The manual linker symbol already uses this name. |
| `0x8009D1D8` | `g_CollisionPlaneTable` | Optional 12-byte plane-equation table for sloped field collision | High | Null selects flat region heights; otherwise collision resolves `A*x + B*y + C*z = D` through `{A, invB, C}` entries. Relocation and reset code already use the name. |

## Naming Rules

- Use the canonical C identifier plus `__asm__("D_xxxxxxxx")` in new or
  converted C. The address remains the external linker symbol.
- Do not rename a raw `D_` symbol solely because it is adjacent to a known
  global. Adjacent state often has different ownership and lifetime.
- Use a subsystem-neutral name only when callers prove ownership across field,
  battle, and task code. Otherwise keep the raw address until its lifecycle is
  understood.
- A name documents an address, not necessarily a complete C type. Keep a raw
  byte pointer when the known callers intentionally use overlapping widths or
  fields; introduce a struct only after its layout has independent evidence.
