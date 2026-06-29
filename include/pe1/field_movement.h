#ifndef PE1_FIELD_MOVEMENT_H
#define PE1_FIELD_MOVEMENT_H

/* Player field movement subsystem (Aya walking the pre-rendered field).
 *
 * Verified live via DuckStation's GDB server: polled the pad, facing angle,
 * motion vector and position while walking/running Aya. See field_actor.h for
 * the FieldActor layout; the movement-relevant offsets are confirmed below.
 *
 * Data flow (per frame):
 *   D_8009D26C (field pad bits)
 *     -> Scene_UpdatePlayerEntity  : pad/analog -> facing angle (+0x3A) + speed,
 *                                    GTE-rotated by the camera into motion (+0x68)
 *     -> Entity_IntegratePositionFull : pos(+0x28) += motion(+0x68) [+ accel/grav/delta]
 *   The active player actor is *(FieldActor**)g_PlayerEntity (the unified "active
 *   actor": same object is the battle participant in battle context).
 */

typedef int   s32;
typedef short s16;

/* ---- Field pad bits (D_8009D26C), confirmed by live walking ---- */
#define FIELD_PAD_UP    0x20   /* north, -Z, facing angle 0x000 */
#define FIELD_PAD_LEFT  0x40   /* west,  -X, facing angle 0x400 */
#define FIELD_PAD_DOWN  0x08   /* south, +Z, facing angle 0x800 */
#define FIELD_PAD_RIGHT 0x10   /* east,  +X, facing angle 0xC00 */
#define FIELD_PAD_RUN   0x40000000 /* high bit set while running (~4.5x speed) */

/* ---- Facing angle (FieldActor +0x3A), 12-bit: 0x1000 == 360 degrees ----
 * 0x000=N(-Z) 0x400=W(-X) 0x800=S(+Z) 0xC00=E(+X); diagonals 0x200/0x600/0xA00/0xE00.
 * (Engine flips to a left-hand system internally via the 0x1400 - angle constant.) */

/* ---- Fixed-point: positions and motion are 16.16 (world int = value >> 16) ---- */
#define FIELD_FIX_SHIFT      16
#define FIELD_WALK_SPEED  262080   /* ~4.0  units/frame (0x3FF80, 16.16) */
#define FIELD_RUN_SPEED  1179360   /* ~18.0 units/frame; diagonals are these / sqrt(2) */
                                   /* observed diagonals: walk 185314, run 833916 */

/* ---- Movement-relevant FieldActor offsets (confirmed in
 * Entity_IntegratePositionFull + live reads); supplements field_actor.h ---- */
#define FIELD_ACTOR_ANGLE        0x3A   /* u16 facing angle (also 0x38/0x3C saved-angle pair) */
#define FIELD_ACTOR_SAVED_ANGLE  0x52   /* u16 prev facing (0x50/0x52/0x54 = saved 0x38/0x3A/0x3C) */
#define FIELD_ACTOR_DELTA_X      0x58   /* s32 extra per-frame displacement added after motion */
#define FIELD_ACTOR_ACCEL_X      0x78   /* s32 constant acceleration added to motion each frame */
#define FIELD_ACTOR_GRAVITY_X    0x88   /* s32 gravity accel, applied to motion when flags&2 (0x98) */
/* (pos 0x28, base/rollback 0x40, motion 0x68, mode 0x0E, flags 0x98 are in field_actor.h) */

/* Key functions / globals:
 *   g_PlayerEntity  void**  active player FieldActor pointer
 *   D_8009D26C  u32     field pad bitmask (see FIELD_PAD_*)
 *   D_8009D2F0  Entity* current entity being dispatched
 *   D_8009D20C  Entity* entity list head (.next at +0x04)
 *   Scene_UpdatePlayerEntity()        pad -> angle + speed -> motion (GTE rotate by camera)
 *   Entity_IntegratePositionFull()    pos += (motion + accel + gravity) ; saves base for rollback
 *   Entity_IntegratePositionConditional()  same, gated by a pause flag
 *   Entity_ResolvePosition()          field-geometry height (Y) + collision via triangle table
 *   Task_MoveTowardPoint(), Task_TurnTowardPointStep()  scripted/NPC movement & turning
 *   Camera base angle: D_800BD020/D_800BD022 ; analog stick: D_800BE9A0/A6/A7
 */

#endif /* PE1_FIELD_MOVEMENT_H */
