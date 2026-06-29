/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

typedef signed char s8;

int g_MenuWidgetColumnLayoutMode;
extern s8 g_MenuWidgetColumnLayoutTable[];
extern s8 D_800A3078[];
extern s8 D_800A30A0[];
extern s8 D_800A30B0[];
extern s8 D_800A30B8[];
extern s8 D_800A30C0[];
extern s8 D_800A30C4[];
extern s8 D_800A3124[];
extern s8 D_800A3134[];

void bzero(void *ptr, int size);

void MenuWidget_SetColumnLayoutMode(int arg0) {
    g_MenuWidgetColumnLayoutMode = arg0;
    if (arg0 == 0) {
        bzero(g_MenuWidgetColumnLayoutTable, 0x120);
        D_800A3078[0] = -1;
        D_800A30A0[0] = -1;
        D_800A30B0[0] = -1;
        D_800A30B8[0] = -1;
        D_800A30C0[0] = -1;
        D_800A30C4[0] = -1;
        D_800A3124[0] = -1;
        D_800A3134[0] = -1;
    }
}
