#include "pe1/aya.h"

extern unsigned short g_AyaStatPeEnergyMax;
extern int g_StatLevelOffsetType3;

extern void Stat_QueryLevelAndSubLevel(int, unsigned int, int *, int);
AyaLevelStats *Aya_LookupLevelStats(int arg0);

extern unsigned int *g_PlayerEntity;
void Inv_RecalcSlotStats(void);

void Aya_DeriveStats(int *out_max, int *out_current) {
    int base;
    AyaLevelStats *stats;

    Stat_QueryLevelAndSubLevel(3, g_AyaStatPeEnergyMax, &base, 0);
    stats = Aya_LookupLevelStats(base + g_StatLevelOffsetType3);

    if (out_max != 0) {
        *out_max = stats->field_10;
    }
    if (out_current != 0) {
        *out_current = stats->field_0c;
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
