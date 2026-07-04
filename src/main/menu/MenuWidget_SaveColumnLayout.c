/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

extern int g_MenuWidgetColumnLayoutMode;
extern signed char g_MenuWidgetColumnLayoutTable[][4];

void MenuWidget_SaveColumnLayout(void *ptr) {
    int index;
    signed char *slot;

    if (g_MenuWidgetColumnLayoutMode == 0 && ((*(int *)((char *)ptr + 0x64) & 0x20) == 0)) {
        return;
    }

    index = *(int *)((char *)ptr + 0x70);
    if ((unsigned int)index < 0x48) {
        slot = g_MenuWidgetColumnLayoutTable[index];
        slot[0] = *(int *)((char *)ptr + 0x44);
        slot[1] = *(int *)((char *)ptr + 0x48);
        slot[2] = *(int *)((char *)ptr + 0x5C);
    }
}
