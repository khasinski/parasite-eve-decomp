
extern unsigned short g_AyaStatPeEnergyMax;
extern int g_StatLevelOffsetType3;

typedef struct AyaStats {
    char pad00[0x0C];
    int current;
    int max;
} AyaStats;

extern void Stat_QueryLevelAndSubLevel(int, unsigned int, int *, int);
extern AyaStats *Aya_LookupLevelStats(int);

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
