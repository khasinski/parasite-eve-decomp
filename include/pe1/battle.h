#ifndef PE1_BATTLE_H
#define PE1_BATTLE_H

#include "common.h"
#include "pe1/field_actor.h"

/* Battle subsystem (ATB combat). Layout reverse-engineered from the battle code
 * (src/main/battle, src/main/main/Battle_) and validated live on real
 * BIOS (DuckStation GDB) by fighting the first Carnegie Hall encounter and
 * cross-checking enemy stats against the wiki (enemy max HP read as 40).
 *
 * These typedefs are the shared ABI vocabulary for the byte-matching build.
 * Direct member access is preferred where it preserves code generation; older
 * compiler-sensitive functions derive their accesses from PE1_OFFSETOF so the
 * layout still has one executable source of truth. Fields marked TENTATIVE need
 * more confirmation.
 */

/* ============================================================================
 * TWO-LAYER MODEL (verified)
 *   g_PlayerEntity (0x8009D254)  -> BattleEntity   (field-actor node)
 *   g_FieldActorListHead         -> BattleEntity   (singly-linked, +0x04 = next)
 *   g_ActiveActor  (0x8009D278)  == *g_PlayerEntity == BattleEntity.core
 *                                -> Combatant in the player-side paths below
 *
 *   Proof: Battle_DispatchInit.c:17        g_ActiveActor = *g_PlayerEntity;
 *          Battle_DispatchSpecialAction.c:30 g_ActiveActor = actor->core;
 *          Battle_ApplyDamage.c:51          g_ActiveActor = *(void**)g_PlayerEntity;
 *          Battle_Set/GetContextField use   ctx = *(u8**)g_PlayerEntity, index ctx.
 *
 *   The old prose notes conflated the two: the side byte / screen coords / a
 *   "player max HP" the original note placed on the combatant are really
 *   BattleEntity fields (the note read them off g_PlayerEntity, the ENTITY). On
 *   the Combatant, +0x1C is genuinely max HP and the side byte is entity+0x0D.
 *
 *   Battle_IsActive(): active iff g_BattleActionQueueCount==0
 *     && (g_GameStateFlags[0] & 2)!=0
 *     && !(combatant->stateFlags & 0x10000)        (stateFlags = Combatant+0x4C)
 *
 *   Quick live probe: in battle iff (*(u32*)0x8009D1A0 & 2);
 *     entity    = *(void**)0x8009D254;
 *     combatant = *(void**)0x8009D278;   (== *entity)
 *     curHP = *(u16*)(combatant + 0x0C); ATB = *(s32*)(combatant + 0x34).
 * ==========================================================================*/

/* Packed battle parameters copied from the encounter attribute globals. The
 * bit ranges in parameterWord are proven, but their gameplay names are still
 * uncertain; keep them packed until those meanings are corroborated.
 */
typedef struct BattleAttributes {
/* 0x00 */ u32 parameterWord; /* 10-bit values at bits 0 and 10; 8-bit at 20 */
/* 0x04 */ u32 effectFlags;   /* attack/status capabilities consumed by hit logic */
} BattleAttributes;

/* Effect descriptor referenced by an enemy's battle-state record. This is a
 * different pointer from Combatant.action at +0x68.
 */
typedef struct EnemyActionEffect {
/* 0x00 */ u8  state;       /* 1=ready; changed to 3/4 by drop resolution */
/* 0x01 */ u8  effectType;  /* Battle_ApplyEnemyAttack dispatch id */
/* 0x02 */ u8  pad_02[0x0A];
/* 0x0C */ u16 power;
/* 0x0E */ u8  category;    /* selects which packed BattleAttributes value applies */
/* 0x0F */ u8  pad_0F;
} EnemyActionEffect;

/* ----------------------------------------------------------------------------
 * Combatant -- the active/player-side stat record. g_ActiveActor points here
 * in the setup and command paths below. Enemy entity cores share several
 * offsets but diverge at +0x10; use EnemyCombatant for those records.
 * --------------------------------------------------------------------------*/
typedef struct Combatant {
/* 0x00 */ u32  coreFlags;     /* bitfield; &0x6000 / &0xC0000 facing, >>13&3 weapon-cat (Battle_GetEnemyContextField.c:28,44,55) */
/* 0x04 */ union {
               s16 fieldId04;  /* SetContextField case1; GetEnemyCtx case0x29 */
               struct {
                   u8 rank;    /* signed rank byte in escape/spawn calculations */
                   u8 field05;
               } bytes;
           } field04;
/* 0x06 */ s16  fieldId06;     /* SetContextField case2 (Battle_SetContextField.c:24) */
/* 0x08 */ s32  exp_or_acc;    /* damage-derived accumulator, clamped to maxAtk@0x28 / 0x10000 (Battle_ApplyDamage.c:92,96) */
/* 0x0C */ u16  curHP;         /* CURRENT HP -- damage & heal land here; clamped to maxHP@0x1C (Battle_ApplyDamage.c:73-85) */
/* 0x0E */ u16  hpMirror;      /* current-HP MIRROR; re-synced from curHP each reset (Battle_ResetEnemyStats.c:42) */
/* 0x10 */ u16  hpAlive;       /* authoritative ">0 == alive" HP (target tests); on player doubles as EXP/level gauge clamped 0x2328 (Battle_BuildTargetList.c:47; Battle_Update.c:155). TENTATIVE width: some reads s32 */
/* 0x12 */ u8   actionMode12;  /* action-mode id table [0x12..0x19], filled per enemy by facing (Battle_SetupEnemyAnims.c:38-46): 4 idle, 0xD special */
/* 0x13 */ s8   actionMode13;
/* 0x14 */ u8   actionMode14;
/* 0x15 */ u8   actionMode15;
/* 0x16 */ u8   actionMode16;
/* 0x17 */ u8   actionMode17;
/* 0x18 */ s8   actionMode18;
/* 0x19 */ s8   actionMode19;
/* 0x1A */ u8   pad_1A[2];
/* 0x1C */ u16  maxHP;         /* MAX HP; curHP clamped to this (Battle_ApplyDamage.c:85; Battle_ResetEnemyStats.c:34) */
/* 0x1E */ s16  stat1E;        /* SetContextField case11 (Battle_SetContextField.c:42) */
/* 0x20 */ s16  stat20;        /* SetContextField case12 (Battle_SetContextField.c:45) */
/* 0x22 */ u16  stat22;        /* base for scaled-offense calc (Battle_SetupEnemyAnims.c:39) */
/* 0x24 */ u16  scaledOffense; /* = stat22 * tbl[anim] / 10 (Battle_SetupEnemyAnims.c:39) */
/* 0x26 */ s16  stat26;        /* SetContextField case18 (Battle_SetContextField.c:50) */
/* 0x28 */ s32  maxAtk;        /* cap for exp_or_acc@0x08 (Battle_ApplyDamage.c:88,95; Battle_StartEncounter.c:116) */
/* 0x2C */ s32  atbStep;       /* per-frame increment subtracted from atbRate */
/* 0x30 */ s32  atbRate;       /* divisor/rate used while charging exp_or_acc */
/* 0x34 */ s32  atbGauge;      /* ATB / "AT" gauge; reset 0, latched 0xF0 (Battle_ResetEnemyStats.c:41; Battle_StartEncounter.c:119) */
/* 0x38 */ u16  subActionStep;
/* 0x3A */ u8   subActionCounter;
/* 0x3B */ u8   subActionPeriod;
/* 0x3C */ u8   pad_3C[0x0C];
/* 0x48 */ s8   knockbackFrames;
/* 0x49 */ u8   knockbackDistance;
/* 0x4A */ s16  knockbackAngle;
/* 0x4C */ s32  stateFlags;    /* PRIMARY status/state bitfield. &0xFF status groups (poison/etc, ApplyDamage cases13-17), 0x10000="battle ended" (Battle_IsActive), 0x80000 special-move guard, 0x100000 acted-this-frame, 0x200000 action-committed (Battle_ApplyPlayerHit.c:53,63; Battle_UpdatePlayerTurn.c:47) */
/* 0x50 */ s16  panelA_val;    /* floating damage-number panel A: {s16 val; u16 x; u16 y; u8 scale; u8 mode}; drawn by Battle_DrawStatusPanel(0, &panelA) (Battle_PhaseHitReaction.c:191) */
/* 0x52 */ u16  panelA_x;      /* x copied from entity projX@0x210 (Battle_PhaseHitReaction.c:188) */
/* 0x54 */ u16  panelA_y;      /* y from entity projY@0x212 */
/* 0x56 */ u8   panelA_timer;  /* countdown; 0=inactive, 0x1E=just-spawned (Battle_PhaseHitReaction.c:185,192) */
/* 0x57 */ u8   panelA_scale;
/* 0x58 */ s16  panelB_val;    /* floating panel B (Battle_PhaseHitReaction.c:203) */
/* 0x5A */ u16  panelB_x;
/* 0x5C */ u16  panelB_y;
/* 0x5E */ u8   panelB_timer;
/* 0x5F */ s8   panelB_flag;
/* 0x60 */ u16  panelAux_val;
/* 0x62 */ u16  panelAux_x;
/* 0x64 */ u16  panelAux_y;
/* 0x66 */ u8   panelAux_timer; /* set to 0x1E when Battle_SubActionStep emits the descriptor */
/* 0x67 */ u8   panelAux_mode;
/* 0x68 */ struct BattleAction *action; /* the queued action/command descriptor; read by nearly every turn func via M2C_FIELD(combatant,void**,0x68) (Battle_Init.c:60; Battle_DrawATBGauge.c:55; Battle_AdvanceTurnSlot.c:70) */
/* 0x6C */ BattleAttributes *attributes;
/* 0x70 */ void *field70;      /* TENTATIVE: no semantic use confirmed yet */
/* 0x74 */ u8   pad_74[0x14];
/* 0x88 */ s32  field88;       /* GetEnemyCtx case19, clamped >=0 (Battle_GetEnemyContextField.c:35) */
/* 0x8C */ u16  field8C;       /* GetEnemyCtx case20 (Battle_GetEnemyContextField.c:41) */
/* 0x8E */ u8   pad_8E[0x3E];
/* 0xCC */ s32  statusFlags2;  /* secondary status bitfield: 0x1000000 petrify-ish, &0x3C000 / &0xC000 / &0x30000 status, &0x30/0xC attack-element gates (Battle_GetEnemyContextField.c:53; Battle_StartEnemyAttackEffect.c:25-64) */
/* 0xD0 */ s16  panelC_val;    /* enemy floating panel C: Battle_DrawStatusPanel(1, &panelC) (Battle_PhaseHitReaction.c:226) */
/* 0xD2 */ u16  panelC_x;
/* 0xD4 */ u16  panelC_y;
/* 0xD6 */ u8   panelC_timer;
                               /* ... struct continues; remaining bytes unmapped */
} Combatant;

/* Enemy entity core. It shares several stat/status offsets with Combatant but
 * diverges by +0x10: notably +0x18 is an EnemyActionEffect pointer, while the
 * active player Combatant stores action-mode bytes there. Do not merge these
 * two views without stronger lifetime evidence.
 */
typedef struct EnemyCombatant {
/* 0x00 */ u32 coreFlags;
/* 0x04 */ union {
               s16 fieldId04;
               struct { u8 rank; u8 field05; } bytes;
           } field04;
/* 0x08 */ s32 field08;
/* 0x0C */ u16 curHP;
/* 0x0E */ u16 hpMirror;
/* 0x10 */ s32 hpAlive;
/* 0x14 */ u8  pad_14[4];
/* 0x18 */ EnemyActionEffect *effect;
/* 0x1C */ u8  pad_1C[0x30];
/* 0x4C */ u32 stateFlags;
/* 0x50 */ u8  pad_50[0x1C];
/* 0x6C */ BattleAttributes *attributes;
/* 0x70 */ u8  pad_70[0x18];
/* 0x88 */ s32 field88;
/* 0x8C */ u16 field8C;
/* 0x8E */ u8  pad_8E[2];
/* 0x90 */ u8  effectChance;
/* 0x91 */ u8  pad_91[3];
/* 0x94 */ u8  effectLevel;
/* 0x95 */ u8  effectDuration;
/* 0x96 */ u8  pad_96[0x0A];
/* 0xA0 */ s16 lootItemId;
/* 0xA2 */ s16 lootItemAux;
/* 0xA4 */ u8  pad_A4[0x28];
/* 0xCC */ u32 statusFlags2;
/* 0xD0 */ u8  pad_D0[8];
} EnemyCombatant;

/* ----------------------------------------------------------------------------
 * BattleAction -- the per-actor action/command descriptor (Combatant.action,
 * i.e. *(void**)(combatant+0x68)). Holds the queued action id and timing.
 * --------------------------------------------------------------------------*/
typedef struct BattleAction {
/* 0x00 */ u8   pad_00[2];
/* 0x02 */ s16  range;         /* compared vs target distance for cycling (Battle_CycleTarget.c:93) */
/* 0x04 */ u8   pad_04[2];
/* 0x06 */ s16  actionId;      /* 5=, 6=fire-weapon, 8=item/special; controls dispatch (Battle_DispatchEntityEffect.c:33-54; Battle_AdvanceTurnSlot.c:67) */
/* 0x08 */ s32  field08;       /* set from anim table (Battle_SetupEnemyAnims.c:52) */
/* 0x0C */ u32  attackWord;    /* &0x3FF = shot/hit countdown (consumed per hit), >>0x14&3 = weapon category (Battle_DrawATBGauge.c:55; Battle_FinalizeAttackResult.c:34) */
/* 0x10 */ u32  turnWord;      /* &0xF = remaining actions, >>4&3 = active turn slot, &0xC0 = action mode, 0x100..0x1000 = attack element (Battle_Init.c:60; Battle_AdvanceTurnSlot.c:70; Battle_StartEnemyAttackEffect.c:26) */
/* 0x14 */ u8   animMode[4];   /* per-facing anim/effect ids [0x14..0x17], filled from a 5-byte table (Battle_Init.c:62; Battle_SetupEntityTarget.c:55) */
                               /* ... continues; rest unmapped */
} BattleAction;

/* ----------------------------------------------------------------------------
 * BattleEntity -- the field-actor / linked-list node. g_PlayerEntity and every
 * g_FieldActorListHead node point here. This is very likely the same field-
 * actor struct used outside battle (see field_actor.h / actor_model.h: the
 * render object lives at +0x1B4). Only battle-relevant offsets are listed; the
 * sparse high offsets (>=0x18C) are proven by address, not adjacency (TENTATIVE
 * ordering).
 * --------------------------------------------------------------------------*/
typedef struct BattleEntity {
/* 0x000 */ void *core; /* Combatant for player paths, EnemyCombatant for enemy entities */
/* 0x004 */ struct BattleEntity *next; /* next in actor list (Battle_BuildTargetList.c:63) */
/* 0x008 */ u8   pad_008[4];
/* 0x00C */ u8   modelIndex;     /* selects a 0xC0-byte action-pointer table (Entity_SetActionMode) */
/* 0x00D */ u8   teamId;        /* group/team id; target must differ/match (Battle_SetupEntityTarget.c:66) */
/* 0x00E */ u8   actionMode;     /* current action/animation mode; <4 and ==0xC are battle-state tests */
/* 0x00F */ u8   animLastFrame;  /* last valid frame index, loaded from action data byte 2 minus one */
/* 0x010 */ u8   pad_010[2];
/* 0x012 */ u16  animStopFrame;  /* optional stop/clamp frame when entityFlags bit 0x200 is set */
/* 0x014 */ s32  animFrame;      /* current animation frame, 16.16 fixed point */
/* 0x018 */ union {
               s32 fixed; /* previous animation frame, 16.16 fixed point */
               struct { u16 fraction; u16 integer; } parts;
           } animPrev;
/* 0x01C */ s32  animStep;       /* signed per-update 16.16 animation increment */
/* 0x020 */ s32  moveFactor;
/* 0x024 */ u16  fieldSfxId;
/* 0x026 */ u16  moveSpeed;
/* 0x028 */ union { s32 fixed; struct { u16 frac; s16 integer; } parts; } posX;
/* 0x02C */ union { s32 fixed; struct { u16 frac; s16 integer; } parts; } posY;
/* 0x030 */ union { s32 fixed; struct { u16 frac; s16 integer; } parts; } posZ;
/* 0x034 */ u8   pad_034[4];
/* 0x038 */ s16  rotationX;
/* 0x03A */ s16  facingAngle;   /* facing/heading, set from target (Battle_UpdateEntityFacing.c:71) */
/* 0x03C */ s16  rotationZ;
/* 0x03E */ u8   pad_03E[2];
/* 0x040 */ s32  baseX;
/* 0x044 */ s32  baseY;
/* 0x048 */ s32  baseZ;
/* 0x04C */ u8   pad_04C[0x1C];
/* 0x068 */ s32  motionX;
/* 0x06C */ s32  motionY;
/* 0x070 */ s32  motionZ;
/* 0x074 */ u8   pad_074[0x24];
/* 0x098 */ u32  entityFlags;   /* &0x40 visible, &0x4000 skip-as-target, &0x40000000 area-target, &0x6000 hit-state (Battle_BuildTargetList.c:46; Battle_SetupEntityTarget.c:60) */
/* 0x09C */ u8  *scriptBase;
/* 0x0A0 */ u8   pad_0A0[0xEC];
/* 0x18C */ struct BattleEntity *parent; /* parent/owner; linked animations recurse through this entity */
/* 0x190 */ u8   pad_190[4];
/* 0x194 */ void *actionCheckFn; /* Entity_CheckActionIdMatch fn ptr (Battle_StartEncounter.c:124) */
/* 0x198 */ u8   pad_198[4];
/* 0x19C */ s32  scriptCursor19C;
/* 0x1A0 */ s32  scriptCursor1A0;
/* 0x1A4 */ void *collisionFace;
/* 0x1A8 */ void *collisionFaceMirror;
/* 0x1AC */ s32  allocationActive; /* nonzero when allocationBlock must be released */
/* 0x1B0 */ void *actionData;    /* current animation/action record */
/* 0x1B4 */ RenderObjectEntity renderObject;
/* 0x270 */ u8   pad_270[0x08];
/* 0x278 */ s32  allocationBlock;
} BattleEntity;

/* ----------------------------------------------------------------------------
 * BattleTarget -- g_BattleTargetList (0x8009E000) entry, CONFIRMED 0xC stride.
 * Built by Battle_BuildTargetList, sorted by Battle_SortTargets, cycled by
 * Battle_CycleTarget. g_BattleTargetCount entries; g_BattleTargetIndex selects.
 * --------------------------------------------------------------------------*/
typedef struct BattleTarget {
/* 0x00 */ struct BattleEntity *actor; /* target entity (Battle_BuildTargetList.c:49) */
/* 0x04 */ s32  dist;                  /* distance to player (Battle_CycleTarget.c:90) */
/* 0x08 */ s16  angle;                 /* Gte_Atan2 angle to player (Battle_BuildTargetList.c:58) */
/* 0x0A */ u8   pad_0A[2];
} BattleTarget;                        /* sizeof == 0xC */

/* These partial records are deliberately padded through their last verified
 * field. Keep the offsets executable: prose comments alone do not protect the
 * shared ABI from an accidental member-width or alignment change. */
PE1_STATIC_ASSERT(PE1_OFFSETOF(Combatant, field04) == 0x04,
                  combatant_field04_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(Combatant, curHP) == 0x0C,
                  combatant_cur_hp_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(Combatant, actionMode12) == 0x12,
                  combatant_action_mode_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(Combatant, atbStep) == 0x2C,
                  combatant_atb_step_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(Combatant, knockbackFrames) == 0x48,
                  combatant_knockback_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(Combatant, subActionStep) == 0x38,
                  combatant_sub_action_step_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(Combatant, panelAux_val) == 0x60,
                  combatant_aux_panel_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(Combatant, action) == 0x68,
                  combatant_action_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(Combatant, attributes) == 0x6C,
                  combatant_attributes_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(Combatant, statusFlags2) == 0xCC,
                  combatant_status_flags2_offset);
PE1_STATIC_ASSERT(sizeof(Combatant) == 0xD8, combatant_partial_size);
PE1_STATIC_ASSERT(sizeof(BattleAction) == 0x18, battle_action_partial_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(BattleAction, turnWord) == 0x10,
                  battle_action_turn_word_offset);
PE1_STATIC_ASSERT(sizeof(BattleAttributes) == 0x08, battle_attributes_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(BattleAttributes, effectFlags) == 0x04,
                  battle_attributes_effect_flags_offset);
PE1_STATIC_ASSERT(sizeof(EnemyActionEffect) == 0x10, enemy_action_effect_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(EnemyActionEffect, power) == 0x0C,
                  enemy_action_effect_power_offset);
PE1_STATIC_ASSERT(sizeof(EnemyCombatant) == 0xD8, enemy_combatant_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(EnemyCombatant, effect) == 0x18,
                  enemy_combatant_effect_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(EnemyCombatant, attributes) == 0x6C,
                  enemy_combatant_attributes_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(EnemyCombatant, effectChance) == 0x90,
                  enemy_combatant_effect_chance_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(EnemyCombatant, lootItemId) == 0xA0,
                  enemy_combatant_loot_item_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(EnemyCombatant, statusFlags2) == 0xCC,
                  enemy_combatant_status_flags_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(EnemyCombatant, curHP) ==
                  PE1_OFFSETOF(Combatant, curHP), combatant_views_hp_match);
PE1_STATIC_ASSERT(PE1_OFFSETOF(EnemyCombatant, attributes) ==
                  PE1_OFFSETOF(Combatant, attributes), combatant_views_attributes_match);
PE1_STATIC_ASSERT(PE1_OFFSETOF(Combatant, curHP) ==
                  PE1_OFFSETOF(FieldActorState, amount), state_views_hp_match);
PE1_STATIC_ASSERT(PE1_OFFSETOF(Combatant, atbStep) ==
                  PE1_OFFSETOF(FieldActorState, progress_step), state_views_step_match);
PE1_STATIC_ASSERT(PE1_OFFSETOF(Combatant, stateFlags) ==
                  PE1_OFFSETOF(FieldActorState, flags), state_views_flags_match);
PE1_STATIC_ASSERT(PE1_OFFSETOF(Combatant, attributes) ==
                  PE1_OFFSETOF(FieldActorState, substate), state_views_substate_match);
PE1_STATIC_ASSERT(sizeof(Combatant) == sizeof(FieldActorState),
                  state_views_size_match);
PE1_STATIC_ASSERT(PE1_OFFSETOF(BattleEntity, posX) == 0x28,
                  battle_entity_pos_x_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(BattleEntity, animFrame) == 0x14,
                  battle_entity_anim_frame_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(BattleEntity, animPrev) == 0x18,
                  battle_entity_anim_prev_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(BattleEntity, parent) == 0x18C,
                  battle_entity_parent_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(BattleEntity, actionData) == 0x1B0,
                  battle_entity_action_data_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(BattleEntity, collisionFace) == 0x1A4,
                  battle_entity_collision_face_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(BattleEntity, posZ) == 0x30,
                  battle_entity_pos_z_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(BattleEntity, facingAngle) == 0x3A,
                  battle_entity_facing_angle_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(BattleEntity, motionX) == 0x68,
                  battle_entity_motion_x_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(BattleEntity, motionZ) == 0x70,
                  battle_entity_motion_z_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(BattleEntity, entityFlags) == 0x98,
                  battle_entity_flags_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(BattleEntity, renderObject.script_param97) == 0x24B,
                  battle_entity_script_param_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(BattleEntity, renderObject.flags_9C) == 0x250,
                  battle_entity_render_flags_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(BattleEntity, renderObject) == 0x1B4,
                  battle_entity_render_object_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(BattleEntity, renderObject.target_x) == 0x268,
                  battle_entity_target_x_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(BattleEntity, renderObject.target_z) == 0x26C,
                  battle_entity_target_z_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(BattleEntity, allocationBlock) == 0x278,
                  battle_entity_allocation_block_offset);
PE1_STATIC_ASSERT(sizeof(BattleEntity) == 0x27C, battle_entity_size);
PE1_STATIC_ASSERT(sizeof(BattleTarget) == 0x0C, battle_target_size);

/* BattleEntity and FieldActor are two naming views of the same runtime object.
 * Keep their independently useful vocabularies, but make layout drift a build
 * error until the remaining call sites can be migrated to one typedef. */
PE1_STATIC_ASSERT(PE1_OFFSETOF(BattleEntity, next) ==
                  PE1_OFFSETOF(FieldActor, next), entity_views_next_match);
PE1_STATIC_ASSERT(PE1_OFFSETOF(BattleEntity, animFrame) ==
                  PE1_OFFSETOF(FieldActor, anim),
                  entity_views_anim_frame_match);
PE1_STATIC_ASSERT(PE1_OFFSETOF(BattleEntity, posX) ==
                  PE1_OFFSETOF(FieldActor, pos_x), entity_views_pos_match);
PE1_STATIC_ASSERT(PE1_OFFSETOF(BattleEntity, moveFactor) ==
                  PE1_OFFSETOF(FieldActor, move_factor), entity_views_move_factor_match);
PE1_STATIC_ASSERT(PE1_OFFSETOF(BattleEntity, facingAngle) ==
                  PE1_OFFSETOF(FieldActor, rot_y), entity_views_rotation_match);
PE1_STATIC_ASSERT(PE1_OFFSETOF(BattleEntity, entityFlags) ==
                  PE1_OFFSETOF(FieldActor, flags), entity_views_flags_match);
PE1_STATIC_ASSERT(PE1_OFFSETOF(BattleEntity, scriptBase) ==
                  PE1_OFFSETOF(FieldActor, script_base), entity_views_script_base_match);
PE1_STATIC_ASSERT(PE1_OFFSETOF(BattleEntity, parent) ==
                  PE1_OFFSETOF(FieldActor, parent), entity_views_parent_match);
PE1_STATIC_ASSERT(PE1_OFFSETOF(BattleEntity, scriptCursor19C) ==
                  PE1_OFFSETOF(FieldActor, script_cursor_19c), entity_views_script_cursor_match);
PE1_STATIC_ASSERT(PE1_OFFSETOF(BattleEntity, renderObject) ==
                  PE1_OFFSETOF(FieldActor, render_object), entity_views_render_match);
PE1_STATIC_ASSERT(PE1_OFFSETOF(BattleEntity, renderObject.table_index) ==
                  PE1_OFFSETOF(FieldActor, render_object.table_index), entity_views_render_field_match);
PE1_STATIC_ASSERT(PE1_OFFSETOF(BattleEntity, allocationBlock) ==
                  PE1_OFFSETOF(FieldActor, allocation_block), entity_views_tail_match);
PE1_STATIC_ASSERT(sizeof(BattleEntity) == sizeof(FieldActor),
                  entity_views_size_match);

int Battle_RollEscapeChance(void);

/* ============================================================================
 * ENTITY TINT COLOR PACKETS (battle/menu)  -- documented, not yet symbol-named.
 *
 * Two BSS regions hold GPU primitive packets whose per-vertex RGB color bytes
 * are recolored every frame to pulse battle entities and status elements. Each
 * is a 4-vertex Gouraud-style primitive (vertex = R,G,B + code byte, then XY/UV)
 * replicated once per on-screen combatant at a fixed stride, indexed by
 * g_ActiveDrawSlot (0x8009CDDC). The named D_800Bxxxx bytes are the R/G/B
 * channels; the gaps are the code byte + XY/UV words. (Per-byte symbols, not a
 * flat array: the code holds individual extern u8 handles per channel.)
 *
 * CLUSTER A -- entity tint, stride 0x48, base packet 0x800B0158 (sibling at
 *   0x800B0134). Seeded in Battle_InitEntityColors.c:81-104 with 4 vertices
 *   alternating {0xFF,0x3D,0x81} (pink) and {0x83,0x13,0x01} (dark orange):
 *     V0 R/G/B = 0x800B0158/59/5A   V1 = 0x800B0160/61/62
 *     V2 R/G/B = 0x800B0168/69/6A   V3 = 0x800B0170/71/72
 *   (init writes a second identical block at 0x800B01A0/A8/B0/B8.) Runtime
 *   recolor picks {0xFF,0x3D,0x81}/{0xC1,0x28,0x41}/{0x83,0x13,0x01} by
 *   g_FrameRngCounter&3 in Battle_PhaseInitEnemyTurn.c:235, Battle_PhaseEndTurn.c:214,
 *   Menu_MainUpdate.c:269 (form *(&D_800B0158 + slot*0x48)).
 *
 * CLUSTER B -- status-bar / hit-reaction tint, stride 0x24, base 0x800B00EC:
 *     V0 R/G/B = 0x800B00EC/ED/EE   V1 = 0x800B00F4/F5/F6
 *     V2 R/G/B = 0x800B00FC/FD/FE   V3 = 0x800B0104/05/06
 *   Recolored {0x46,0x82,0x9F}/{0xFF,0xF9,0x00}/{0xA3,0xBE,0x50} in
 *   Battle_PhaseHitReaction.c:66, Battle_PhaseInitEnemyTurn.c:111. (sibling
 *   packets at 0x800B0111.. and 0x800B0130..)
 * ==========================================================================*/

/* ============================================================================
 * PERSISTENT STATS (out of battle) -- see aya.h for the full AyaSaveState.
 *   Aya's saved stats are the g_AyaStat* block at 0x800C0E00 (max HP +0x06,
 *   current HP +0x08, level +0x0A, bonus points +0x10, offense +0x2A, ...).
 *   In-battle damage does NOT touch this block; the combatant HP is synced
 *   back to it at battle end, so writing HP here is the clean persistent way.
 *
 * STATIC ENEMY DEFINITION (not yet located): the runtime Combatant does NOT
 *   hold Level / EXP / BP / status resistances -- those come from a per-enemy
 *   DEFINITION record streamed from PE.IMG into a separate table (TODO: locate;
 *   likely reached via BattleEntity.defPtr@0x238).
 * ==========================================================================*/

#endif /* PE1_BATTLE_H */
