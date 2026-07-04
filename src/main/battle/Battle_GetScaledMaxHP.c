extern int g_StatScaleBase;
unsigned short *Aya_LookupLevelStats(int arg0);

int Battle_GetScaledMaxHP(int arg0) {
    unsigned short value = *Aya_LookupLevelStats(arg0);
    return ((g_StatScaleBase + 0x14) * value) / 20;
}
