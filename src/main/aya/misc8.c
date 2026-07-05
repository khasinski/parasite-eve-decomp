extern unsigned short g_AyaStatPeEnergyMax;
extern int g_StatLevelOffsetType3;

typedef struct AyaStats {
    char pad00[0x0C];
    int current;
    int max;
} AyaStats;

extern void Stat_QueryLevelAndSubLevel(int, unsigned int, int *, int);
void *Aya_LookupLevelStats(int arg0);

extern unsigned int *g_PlayerEntity;
void Inv_RecalcSlotStats(void);

void Aya_DeriveStats(int *out_max, int *out_current) {
    int base;
    AyaStats *stats;

    Stat_QueryLevelAndSubLevel(3, g_AyaStatPeEnergyMax, &base, 0);
    stats = Aya_LookupLevelStats(base + g_StatLevelOffsetType3);

    if (out_max != 0) {
        *out_max = stats->max;
    }
    if (out_current != 0) {
        *out_current = stats->current;
    }
}

void Battle_SyncEquipSlots(void) {
    unsigned int *outer;
    unsigned short *ptr;
    unsigned short value;

    Inv_RecalcSlotStats();
    outer = g_PlayerEntity;
    if (outer != 0) {
        ptr = (unsigned short *)outer[0];
        if (ptr != 0) {
            value = ptr[0xE];
            ptr[7] = value;
            ptr[6] = value;
        }
    }
}
