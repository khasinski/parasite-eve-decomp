/* CC1_FLAGS: -g3 */
/* MASPSX_FLAGS: -G8 */

typedef signed char s8;

void bzero(void *ptr, int size);

extern s8 g_MenuWidgetColumnLayoutTable[];
extern s8 D_800A3078[];
extern s8 D_800A30A0[];
extern s8 D_800A30B0[];
extern s8 D_800A30B8[];
extern s8 D_800A30C0[];
extern s8 D_800A30C4[];
extern s8 D_800A3124[];
extern s8 D_800A3134[];

void Sys_InitStateBuffer(void) {
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
