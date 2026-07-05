/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: --use-comm-section -G8 */

extern int g_MenuWidgetColumnLayoutMode;
extern signed char g_MenuWidgetColumnLayoutTable[][4];

void MenuWidget_ApplyColumnLayout(void *arg0) {
    signed char *entry;
    int flag;

    if (arg0 == 0) {
        return;
    }
    if (*(int *)((char *)arg0 + 0x70) < 0) {
        return;
    }

    entry = g_MenuWidgetColumnLayoutTable[*(int *)((char *)arg0 + 0x70)];
    flag = g_MenuWidgetColumnLayoutMode;
    *(int *)((char *)arg0 + 0x44) = entry[0];
    if ((flag != 0) || ((*(int *)((char *)arg0 + 0x64) & 0x20) != 0)) {
        *(int *)((char *)arg0 + 0x48) = entry[1];
        if (*(int *)((char *)arg0 + 0x58) <= *(int *)((char *)arg0 + 0x48)) {
            *(int *)((char *)arg0 + 0x48) = *(int *)((char *)arg0 + 0x58) - 1;
        }
        if ((*(int *)((char *)arg0 + 0x68) != 0) &&
            (*(int *)((char *)arg0 + 0x44) == 1) &&
            (*(int *)((char *)arg0 + 0x48) == (*(int *)((char *)arg0 + 0x58) - 1))) {
            *(int *)((char *)arg0 + 0x44) = 0;
        }
        *(int *)((char *)arg0 + 0x5C) = entry[2];
    }
}

void MenuWidget_SetColumnLayout(void *arg0, int arg1) {
    signed char *entry;
    int flag;

    *(int *)((char *)arg0 + 0x70) = arg1;
    if (arg0 == 0) {
        return;
    }
    if (arg1 < 0) {
        return;
    }

    entry = g_MenuWidgetColumnLayoutTable[arg1];
    flag = g_MenuWidgetColumnLayoutMode;
    *(int *)((char *)arg0 + 0x44) = entry[0];
    if ((flag != 0) || ((*(int *)((char *)arg0 + 0x64) & 0x20) != 0)) {
        *(int *)((char *)arg0 + 0x48) = entry[1];
        if (*(int *)((char *)arg0 + 0x58) <= *(int *)((char *)arg0 + 0x48)) {
            *(int *)((char *)arg0 + 0x48) = *(int *)((char *)arg0 + 0x58) - 1;
        }
        if ((*(int *)((char *)arg0 + 0x68) != 0) &&
            (*(int *)((char *)arg0 + 0x44) == 1) &&
            (*(int *)((char *)arg0 + 0x48) == (*(int *)((char *)arg0 + 0x58) - 1))) {
            *(int *)((char *)arg0 + 0x44) = 0;
        }
        *(int *)((char *)arg0 + 0x5C) = entry[2];
    }
}
