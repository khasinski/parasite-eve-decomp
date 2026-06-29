extern int g_GameTimeTable[];

void Menu_SetTextCursorRect(int arg0, int arg1, int arg2, int arg3);
void Render_SetupColorTable(int arg0, int arg1, short *arg2);

int Task_DrawSaveOverlay(unsigned short **arg0) {
    Menu_SetTextCursorRect(*arg0[0], *arg0[1], *arg0[2], *arg0[3]);
    return 1;
}

int Task_ResetEntityAnimSlot(short **arg0) {
    short local[8];

    local[0] = -1;
    Render_SetupColorTable(*arg0[0], 1, local);
    return 1;
}

int Task_GetFloorTableValue(int **arg0) {
    *arg0[1] = g_GameTimeTable[*arg0[0] * 3];
    return 1;
}
