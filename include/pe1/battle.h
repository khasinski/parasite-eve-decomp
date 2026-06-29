#ifndef PE1_BATTLE_H
#define PE1_BATTLE_H

/* Battle subsystem (ATB combat). Layout reverse-engineered from the battle code
 * (src/main/battle, src/main/main/Battle_) and validated live on real
 * BIOS (DuckStation GDB) by fighting the first Carnegie Hall encounter and
 * cross-checking enemy stats against the wiki (enemy max HP read as 40).
 *
 * These typedefs are DOCUMENTATION for the PC port: the byte-matching build
 * still accesses these objects through M2C_FIELD()/raw offsets, so nothing here
 * is compiled into main.exe (no .c includes this header). Offsets are verified
 * against the cited accessors; fields marked TENTATIVE need more confirmation.
 */

typedef unsigned char  u8;
typedef unsigned short u16;
typedef signed char    s8;
typedef short          s16;
typedef int            s32;
typedef unsigned int   u32;

/* ============================================================================
 * TWO-LAYER MODEL (verified)
 *   g_PlayerEntity (0x8009D254)  -> BattleEntity   (field-actor node)
 *   g_FieldActorListHead         -> BattleEntity   (singly-linked, +0x04 = next)
 *   g_ActiveActor  (0x8009D278)  == *g_PlayerEntity == BattleEntity.core
 *                                -> Combatant       (the core stat/state record)
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

/* ----------------------------------------------------------------------------
 * Combatant -- the core stat/state record. g_ActiveActor points here. Same
 * layout for the player (Aya) and enemies. Indexed by Battle_ApplyDamage,
 * Battle_ResetEnemyStats, Battle_IsActive, Battle_Set/GetContextField (ctx),
 * Battle_DrawATBGauge, etc.
 * --------------------------------------------------------------------------*/
typedef struct Combatant {
/* 0x00 */ u32  coreFlags;     /* bitfield; &0x6000 / &0xC0000 facing, >>13&3 weapon-cat (Battle_GetEnemyContextField.c:28,44,55) */
/* 0x04 */ s16  fieldId04;     /* SetContextField case1; GetEnemyCtx case0x29 (Battle_SetContextField.c:21) */
/* 0x06 */ s16  fieldId06;     /* SetContextField case2 (Battle_SetContextField.c:24) */
/* 0x08 */ s32  exp_or_acc;    /* damage-derived accumulator, clamped to maxAtk@0x28 / 0x10000 (Battle_ApplyDamage.c:92,96) */
/* 0x0C */ u16  curHP;         /* CURRENT HP -- damage & heal land here; clamped to maxHP@0x1C (Battle_ApplyDamage.c:73-85) */
/* 0x0E */ u16  hpMirror;      /* current-HP MIRROR; re-synced from curHP each reset (Battle_ResetEnemyStats.c:42) */
/* 0x10 */ u16  hpAlive;       /* authoritative ">0 == alive" HP (target tests); on player doubles as EXP/level gauge clamped 0x2328 (Battle_BuildTargetList.c:47; Battle_Update.c:155). TENTATIVE width: some reads s32 */
/* 0x12 */ s8   actionMode12;  /* action-mode id table [0x12..0x19], filled per enemy by facing (Battle_SetupEnemyAnims.c:38-46): 4 idle, 0xD special */
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
/* 0x24 */ s16  scaledOffense; /* = stat22 * tbl[anim] / 10 (Battle_SetupEnemyAnims.c:39) */
/* 0x26 */ s16  stat26;        /* SetContextField case18 (Battle_SetContextField.c:50) */
/* 0x28 */ s32  maxAtk;        /* cap for exp_or_acc@0x08 (Battle_ApplyDamage.c:88,95; Battle_StartEncounter.c:116) */
/* 0x2C */ u8   pad_2C[8];
/* 0x34 */ s32  atbGauge;      /* ATB / "AT" gauge; reset 0, latched 0xF0 (Battle_ResetEnemyStats.c:41; Battle_StartEncounter.c:119) */
/* 0x38 */ u8   pad_38[0x14];
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
/* 0x60 */ u8   pad_60[6];
/* 0x66 */ s8   field66;       /* cleared on reset (Battle_ResetEnemyStats.c:92) */
/* 0x67 */ u8   pad_67[1];
/* 0x68 */ struct BattleAction *action; /* the queued action/command descriptor; read by nearly every turn func via M2C_FIELD(combatant,void**,0x68) (Battle_Init.c:60; Battle_DrawATBGauge.c:55; Battle_AdvanceTurnSlot.c:70) */
/* 0x6C */ void *ptr6C;        /* TENTATIVE */
/* 0x70 */ void *actionDesc70; /* action descriptor (target+effect) set on commit. TENTATIVE */
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
/* 0x000 */ struct Combatant   *core;  /* pointer to the Combatant record (Battle_DispatchSpecialAction.c:30; Battle_BuildTargetList.c:44) */
/* 0x004 */ struct BattleEntity *next; /* next in actor list (Battle_BuildTargetList.c:63) */
/* 0x008 */ u8   pad_008[5];
/* 0x00D */ u8   teamId;        /* group/team id; target must differ/match (Battle_SetupEntityTarget.c:66) */
/* 0x00E */ u8   animState;     /* <4 / ==0xC tests (Battle_ApplyPlayerHit.c:69; Battle_UpdatePlayerTurn.c:46) */
/* 0x00F */ u8   animFrame;     /* current anim frame; ==animFrameMax => action complete (Battle_ApplyPlayerHit.c:61; Battle_StepLevelUp.c:75) */
/* 0x010 */ u8   pad_010[6];
/* 0x016 */ u16  levelOrFlag16; /* ==0xA gate at level-up (Battle_StepLevelUp.c:57) TENTATIVE */
/* 0x018 */ u8   pad_018[2];
/* 0x01A */ u16  animFrameMax;  /* total anim frames (Battle_ApplyPlayerHit.c:61; Battle_UpdatePlayerTurn.c:87) */
/* 0x01C */ u8   pad_01C[0x0E];
/* 0x02A */ s16  worldX;        /* world position X (Battle_ApplyDamage.c:56) */
/* 0x02C */ u8   pad_02C[2];
/* 0x02E */ s16  worldY;        /* world position Y (Battle_ApplyDamage.c:57) */
/* 0x030 */ u8   pad_030[2];
/* 0x032 */ s16  worldZ;        /* world position Z (Battle_ApplyDamage.c:58) */
/* 0x034 */ u8   pad_034[6];
/* 0x03A */ s16  facingAngle;   /* facing/heading, set from target (Battle_UpdateEntityFacing.c:71) */
/* 0x03C */ u8   pad_03C[0x5C];
/* 0x098 */ u32  entityFlags;   /* &0x40 visible, &0x4000 skip-as-target, &0x40000000 area-target, &0x6000 hit-state (Battle_BuildTargetList.c:46; Battle_SetupEntityTarget.c:60) */
/* 0x09C */ u8   pad_09C[0xF0];
/* 0x18C */ struct BattleEntity *owner; /* parent/owner (projectile->shooter) (Battle_SetupEntityTarget.c:80; Battle_StepEntityDeath.c:42) TENTATIVE */
/* 0x190 */ u8   pad_190[4];
/* 0x194 */ void *actionCheckFn; /* Entity_CheckActionIdMatch fn ptr (Battle_StartEncounter.c:124) */
/* 0x198 */ u8   pad_198[0x1C];
/* 0x1B4 */ u8   renderObject[0x5C]; /* GPU render/color sub-block; passed to Render_FadeEntityColor / Battle_DrawStatusOverlay (entity+0x1B4). See actor_model.h */
/* 0x210 */ s16  projX;         /* projected screen X, source for panel x (Battle_PhaseHitReaction.c:188) */
/* 0x212 */ s16  projY;         /* projected screen Y, source for panel y (Battle_PhaseHitReaction.c:189) */
/* 0x214 */ u8   pad_214[0x24];
/* 0x238 */ void *defPtr;       /* ptr to a definition record; ->+0x18 baseline (Battle_StartEncounter.c:125) TENTATIVE */
/* 0x23C */ u8   pad_23C[0x14];
/* 0x250 */ u16  hideFlags;     /* |= 0x20 to cull on death (Battle_StepEntityDeath.c:60) */
/* 0x252 */ u8   field252;      /* (Battle_StepEntityDeath.c:79) */
/* 0x253 */ u8   pad_253[0x15];
/* 0x268 */ s16  targetX;       /* world coord for target geometry & effect spawn (Battle_BuildTargetList.c:54; Battle_StartEnemyAttackEffect.c:35) */
/* 0x26A */ s16  targetY;
/* 0x26C */ s16  targetZ;
                                /* ... continues; rest unmapped */
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
