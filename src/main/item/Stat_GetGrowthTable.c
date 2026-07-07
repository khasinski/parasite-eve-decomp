/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

extern int g_StatGrowthTable[];

int Stat_GetGrowthTable(int arg0) {
    return g_StatGrowthTable[0] + (int)((char *)g_StatGrowthTable - 0x10) + (arg0 << 9);
}
