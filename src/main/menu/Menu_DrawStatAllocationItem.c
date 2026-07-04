/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

extern int g_BonusPointBarAnimProgress;
extern int g_BonusPointStatDeltas[];
extern int g_BonusPointStatMultipliers[];

void Stat_QueryLevelAndSubLevel(int, int, int *, int);
void Draw_OffsetCursor(int, int);
void Draw_AllocSprite(int);
void Draw_PrintNumberWidth2(int);

void Menu_DrawStatAllocationItem(int arg0) {
    int value;
    int local;

    arg0 += 1;
    value = g_BonusPointStatDeltas[arg0] + ((g_BonusPointBarAnimProgress * g_BonusPointStatMultipliers[arg0]) >> 7);

    Draw_OffsetCursor(6, 0);
    Draw_AllocSprite(arg0 + 0x8C);
    Draw_OffsetCursor(0x4A, 0);
    Stat_QueryLevelAndSubLevel(arg0, value, &local, 0);
    Draw_PrintNumberWidth2(local + 1);
}
