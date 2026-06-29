#ifndef PE1_AYA_H
#define PE1_AYA_H

/* Aya character / save / stat subsystem.
 *
 * Reverse-engineered layout of Aya's persistent character state and the
 * level/stat derivation tables, captured for the PC port. Offsets and types
 * are verified against configs/USA/sym.main.txt and the accessor functions
 * cited per field; fields marked TENTATIVE still need runtime confirmation
 * (read the live block during gameplay and compare with the status screen).
 *
 * Field names that already exist in sym.main.txt are used as-is; anonymous
 * D_800C0Exx fields carry a proposed name in the comment until renamed.
 *
 * ARCHITECTURE NOTE (confirmed by live GDB poke + screenshot): this block is a
 * MIX of two kinds of fields. PERSISTENT save fields (total_exp, level,
 * bonus_points, equipped slots, parasite_spell_flags, inventory) hold their
 * written value. DERIVED CACHE fields (current/max HP at 0x06/0x08 and the
 * stat block at 0x2A..0x34) are recomputed from the persistent state + the
 * level tables on resume/refresh, so writing them directly does not stick
 * (e.g. poking HP=999 reverted to 45/45; raising level recomputed OFFENSE).
 * For the PC port, treat HP/stats as derived from (level, EXP, bonus alloc).
 */

typedef unsigned char  u8;
typedef unsigned short u16;
typedef signed char    s8;
typedef int            s32;
typedef unsigned int   u32;

/* ------------------------------------------------------------------------- */
/* Persistent character/save state, based at 0x800C0E00.                     */
/* Written at new game (Inv_InitNewGameInventory) and on level-up            */
/* (Aya_SetTotalExp); read across menu/battle/inventory code.                */
/* ------------------------------------------------------------------------- */
typedef struct AyaSaveState {
    /* 0x00 */ u32 total_exp;            /* g_AyaSaveTotalExp: total accumulated EXP [CONFIRMED 0 at new game] */
    /* 0x04 */ u8  pad_04[0x02];
    /* 0x06 */ u16 max_hp;               /* g_AyaHpMax  [CONFIRMED] max HP (status "45/45" right side) */
    /* 0x08 */ u16 current_hp;           /* g_AyaHpCurrent  [CONFIRMED] current HP (status left side); synced from battle slot +0x0C */
    /* 0x0A */ u8  level;                /* g_AyaSaveLevel: 0-based, status shows +1 [CONFIRMED 0 -> "LEVEL 1"]; clamp 0..0x62 */
    /* 0x0B */ u8  pad_0B;
    /* 0x0C */ u32 inventory_slot_count; /* g_AyaInventorySlotCount [CONFIRMED 10 at new game]; <=0x32 */
    /* 0x10 */ u32 bonus_points;         /* g_AyaBonusPoints [CONFIRMED 0 -> "BONUS POINT 0"]; clamp 0x1869F */
    /* 0x14 */ u8  pad_14[0x0A];
    /* 0x1E */ u16 pe_bonus_pool;        /* D_800C0E1E  TENTATIVE: pool cleared to 0 on level-up (Aya_SetTotalExp) */
    /* 0x20 */ s8  equipped_weapon_slot; /* g_AyaEquippedWeaponSlot  [CONFIRMED 0 -> "M84F"] equipped weapon inventory slot */
    /* 0x21 */ u8  pad_21;
    /* 0x22 */ s8  equipped_armor_slot;  /* g_AyaEquippedArmorSlot  [CONFIRMED 1 -> "N Vest"] equipped armor inventory slot */
    /* 0x23 */ u8  pad_23;
    /* 0x24 */ u32 parasite_spell_flags; /* g_AyaParasiteSpellFlags: unlocked-spell bitmask [CONFIRMED 0x1 at new game] */

    /* 0x28..0x35: 7 u16 stat-allocation LEVELS (0-based, status screen shows +1),
     * written as one 7-entry array in Inv_InitNewGameInventory (loop from
     * func_8005DB8C) and read via the g_AyaStat* accessors. These are the
     * bonus-point allocation levels per category (all 0 at new game ->
     * every status stat shows "1"), NOT the final combat values. */
    /* 0x28 */ u16 stat_agility;         /* g_AyaStatAgility  Battle_GetAgilityBonus; 7th/hidden stat, NOT on the status screen */
    /* 0x2A */ u16 stat_offense;         /* g_AyaStatOffense        [CONFIRMED -> "OFFENSE"] */
    /* 0x2C */ u16 stat_defense;         /* g_AyaStatDefense        [CONFIRMED -> "DEFENSE"] */
    /* 0x2E */ u16 stat_pe_energy_max;   /* g_AyaStatPeEnergyMax    [CONFIRMED -> "P.ENERGY"] */
    /* 0x30 */ u16 stat_status_recovery; /* g_AyaStatStatusRecovery [CONFIRMED -> "STATUS RECOVER"] */
    /* 0x32 */ u16 stat_active_time_rate;/* g_AyaStatActiveTimeRate [CONFIRMED -> "ACTIVE TIME"] (Aya_GetActiveTimeRate) */
    /* 0x34 */ u16 stat_item_capacity;   /* g_AyaStatItemCapacity   [CONFIRMED -> "ITEM CAPACITY"] */

    /* 0x36 */ u8  pad_36[0x0A];
    /* 0x40 */ u16 menu_clamp_value;     /* D_800C0E40  [CONFIRMED 0x3D] set via Menu_ClampRange(0x3D) at init */
    /* 0x42 */ u8  pad_42[0x06];         /* 0x44..0x46 observed = 40 40 40 (per-category cursor?) */
    /* 0x48 */ u16 inventory_items[1];   /* g_AyaInventoryItems: variable-length item-id array */
} AyaSaveState;                          /* extends past 0x48 with the item list */

/* ------------------------------------------------------------------------- */
/* Per-level stat record. The level table base is reached via                */
/* Aya_LookupLevelStats(level) = tableBase + level*0x18 (0x63 levels).       */
/* Field layout is TENTATIVE, inferred from Inv_RecalcSlotStats /            */
/* Aya_DeriveStats indexing; confirm against the runtime table.              */
/* ------------------------------------------------------------------------- */
typedef struct AyaLevelStats {
    /* 0x00 */ u16 hp;        /* [CONFIRMED] base HP per level: 45,49,53,64,67,72,92,... (L0 HP=45 == status "45/45") */
    /* 0x02 */ u16 offense;   /* [CONFIRMED] base offense: 30,35,40,45,55,60,65,... (NOT the status "OFFENSE 1" allocation level) */
    /* 0x04 */ u16 defense;   /* [CONFIRMED] base defense: 10,15,20,25,40,50,65,... */
    /* 0x06 */ u16 field_06;  /* TENTATIVE: grows +257/level (2610,2867,3124,...) */
    /* 0x08 */ s32 field_08;  /* TENTATIVE: looks 16.16 fixed (0x500000=80.0, +4.0/level) -- likely P.Energy */
    /* 0x0C */ s32 field_0c;  /* TENTATIVE: ~8500, slow growth (status recovery?) */
    /* 0x10 */ s32 field_10;  /* TENTATIVE: 7864,8650,11337,... (active-time / speed?) */
    /* 0x14 */ u16 field_14;  /* TENTATIVE: constant 15 (active-time rate?) */
    /* 0x16 */ u8  field_16;  /* TENTATIVE: constant 10 (base item capacity?) */
    /* 0x17 */ u8  field_17;  /* TENTATIVE: constant 0 */
} AyaLevelStats;             /* 0x18 bytes. Table base = *(u32*)0x800A803C + 0x800A8028, indexed level*0x18 (0..0x62). */

/* EXP threshold table (Aya_GetLevelExpTable): base = *(u32*)0x800A8040 + 0x800A8028,
 * 4 bytes/level, cumulative EXP to REACH each level [CONFIRMED live]:
 *   L0=0 L1=3 L2=8 L3=19 L4=28 L5=41 L6=68 L7=104 L8=142 L9=184 L10=240 L11=325 L12=428...
 * The status screen "NEXT LEVEL" shows exptable[current_level+1] (e.g. 3 at L1, 240 at L10). */

/* Stat derivation entry points (see src/main/aya, src/main/item):
 *   void *Aya_GetLevelExpTable(void);        // 0x8005DBF8 alias; 4 bytes/level EXP thresholds
 *   AyaLevelStats *Aya_LookupLevelStats(int level); // tableBase + level*0x18
 *   int  Stat_BinarySearch(...);             // EXP -> level
 *   void Stat_QueryLevelAndSubLevel(int cat, u32 value, int *outLevel, int flag);
 *   void Aya_DeriveStats(int *out_max, int *out_current);
 *   int  Battle_GetScaledMaxHP(void);        // (D_800A1B30+0x14)*levelHp/20
 *   void Inv_RecalcSlotStats(void);          // pushes save-state stats into the active battle slot
 * Stat scaling parameters live at D_800A1B30..D_800A1B44 (per-category offsets).
 */

#endif /* PE1_AYA_H */
