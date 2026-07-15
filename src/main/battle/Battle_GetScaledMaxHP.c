extern int g_StatScaleBase __asm__("D_800A1B30");

unsigned short *Aya_LookupLevelStats(int level);

int Battle_GetScaledMaxHP(int level) {
    unsigned short maxHp;

    maxHp = *Aya_LookupLevelStats(level);
    return ((g_StatScaleBase + 0x14) * maxHp) / 20;
}
