/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: --use-comm-section -G8 */

typedef signed char s8;typedef unsigned char u8;typedef short s16;typedef unsigned short u16;typedef int s32;typedef unsigned int u32;typedef long long s64;

extern void Stat_QueryDistanceAndSubLevel(s32 arg0, s32 arg1, s32 *out0, s32 *out1);
extern void Draw_AllocTexturedRectAlt(s32 arg0, s32 arg1);

extern s32 g_BonusPointBarAnimProgress;
extern s32 g_BonusPointStatDeltas[];
extern s32 g_BonusPointStatMultipliers[];
extern s32 g_MenuStatBarWidths[];
extern s32 g_MenuStatBarSettleTimers[];

int g_MenuItemContextFlag;

void Menu_UpdateStatBarAnimation(s32 arg0) {
    s32 sp10;
    s32 sp14;
    s32 current;
    s32 timer;

    Stat_QueryDistanceAndSubLevel(
        arg0,
        g_BonusPointStatDeltas[arg0] + ((g_BonusPointBarAnimProgress * g_BonusPointStatMultipliers[arg0]) >> 7),
        &sp10,
        &sp14);

    current = g_MenuStatBarWidths[arg0];
    if (sp14 >= current) {
        g_MenuStatBarWidths[arg0] = (current + sp14) >> 1;
    } else if (current < 0x30) {
        g_MenuStatBarWidths[arg0] = 0x30;
        g_MenuStatBarSettleTimers[arg0] = 4;
    } else {
        timer = g_MenuStatBarSettleTimers[arg0];
        if (timer > 0) {
            timer -= 2;
        }
        g_MenuStatBarSettleTimers[arg0] = timer;
        if (timer == 0) {
            g_MenuStatBarWidths[arg0] = sp14;
        }
    }

    Draw_AllocTexturedRectAlt(sp10, g_MenuStatBarWidths[arg0]);
}

int Menu_ClampRange(int arg0) {
    arg0 &= 0x1FF;
    g_MenuItemContextFlag = arg0;
    if (arg0 == 0) {
        g_MenuItemContextFlag = 0x1FF;
    }
    return 0;
}

int Menu_GetItemContextFlag(void) {
    return g_MenuItemContextFlag;
}
