/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
#include "pe1/inventory.h"

extern int g_MenuSpendArrowDirection;
extern int g_BonusPointSpendStat;
extern int g_BonusPointSpendWorkingValue;
extern int g_BonusPointSpendCurrentValue;
extern int D_8009CF18;
void Draw_OffsetCursor(int, int);
void Draw_AllocSprite(int);
void Draw_PrintNumberWidth5(int);
void Draw_PrintNumberWidth4(int);
void Draw_PrintNumberWidth4Unk(int);
void Draw_PrintSignedNumberWidth4(int);
void Draw_PrintNumberWidth2(int);
void Draw_AllocTexturedRectAlt(int, int);
void Stat_QueryLevelAndSubLevel(int, int, int *, int *);
void Stat_QueryDistanceAndSubLevel(int, int, int *, int *);

void Menu_DrawBonusPointSpendPanel(void) {
    u8 *stat;
    int value;
    int mode;
    int level;
    int distance;
    int sublevel;
    register int sprite_index asm("$2");

    Draw_OffsetCursor(0x14, 0x15);
    Draw_AllocSprite(g_MenuSpendArrowDirection + 0x4D);
    Draw_OffsetCursor(-0xE, -0x11);
    Draw_AllocSprite(0x93);

    if (g_BonusPointSpendStat < 3) {
        Draw_OffsetCursor(0x48, 0);
        Draw_PrintNumberWidth5(g_BonusPointSpendWorkingValue);
        Draw_OffsetCursor(-0x75, 0x1E);
        if (D_8009CF18 != 0) {
            sprite_index = g_BonusPointSpendStat;
            Draw_AllocSprite(sprite_index + 0x7C);
        } else {
            sprite_index = g_BonusPointSpendStat;
            Draw_AllocSprite(sprite_index + 0x7F);
        }
        Draw_OffsetCursor(0x1E, 0);
        mode = g_BonusPointSpendStat;
        switch (mode) {
        case 0: {
            stat = &D_800A1A00.baseStats[0];
            /* Keep the bonus load relative to the selected stat pointer. */
            asm volatile("" : "=r"(stat) : "0"(stat));
            value = *stat + *(s16 *)(stat + 7);
            if (value >= 1000) value = 999;
            Draw_PrintNumberWidth4(value);
            Draw_OffsetCursor(4, 2);
            Draw_PrintNumberWidth4Unk(*stat);
            Draw_OffsetCursor(5, 0);
            Draw_PrintSignedNumberWidth4(*(s16 *)(stat + 7));
            break;
        }
        case 1: {
            stat = &D_800A1A00.baseStats[1];
            asm volatile("" : "=r"(stat) : "0"(stat));
            value = *stat + *(s16 *)(stat + 8);
            if (value >= 1000) value = 999;
            Draw_PrintNumberWidth4(value);
            Draw_OffsetCursor(4, 2);
            Draw_PrintNumberWidth4Unk(*stat);
            Draw_OffsetCursor(5, 0);
            Draw_PrintSignedNumberWidth4(*(s16 *)(stat + 8));
            break;
        }
        case 2: {
            stat = &D_800A1A00.baseStats[2];
            asm volatile("" : "=r"(stat) : "0"(stat));
            value = *stat + *(s16 *)(stat + 9);
            if (value >= 1000) value = 999;
            Draw_PrintNumberWidth4(value);
            Draw_OffsetCursor(4, 2);
            Draw_PrintNumberWidth4Unk(*stat);
            Draw_OffsetCursor(5, 0);
            Draw_PrintSignedNumberWidth4(*(s16 *)(stat + 9));
            break;
        }
        }
        Draw_OffsetCursor(-0x2D, -0xA);
        Draw_AllocSprite(0x87);
        Draw_OffsetCursor(0x19, 0);
        Draw_AllocSprite(0x88);
    } else {
        Draw_OffsetCursor(0x5C, 0);
        Draw_PrintNumberWidth5(g_BonusPointSpendWorkingValue);
        Draw_OffsetCursor(-0x8A, 0x1F);
        Draw_AllocSprite(g_BonusPointSpendStat + 0x8C);
        Draw_OffsetCursor(0x42, -1);
        Stat_QueryLevelAndSubLevel(g_BonusPointSpendStat,
                                   g_BonusPointSpendCurrentValue, &level, 0);
        Draw_PrintNumberWidth2(level + 1);
        Stat_QueryDistanceAndSubLevel(g_BonusPointSpendStat,
                                      g_BonusPointSpendCurrentValue, &distance, &sublevel);
        Draw_OffsetCursor(2, 0);
        Draw_AllocTexturedRectAlt(distance, sublevel);
    }
}
