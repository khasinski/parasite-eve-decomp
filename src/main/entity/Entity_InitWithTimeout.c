/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

extern int g_SceneDataTable0;
extern char *g_TaskNodePool;
extern unsigned short D_8009D2A4[];

void Render_BeginSceneLoad(void);
void Menu_OpenEquipSlotSwap(int arg0);

int Entity_InitWithTimeout(int **arg0) {
    char *state = g_TaskNodePool;
    unsigned short flags = *(unsigned short *)(state + 8);

    if ((flags & 0x20) == 0) {
        *(unsigned short *)(state + 8) = flags | 0x20;
        Render_BeginSceneLoad();
        g_SceneDataTable0 -= 0x10;
        *(int *)(g_TaskNodePool + 0x10) = 1;
        return 0;
    }

    if ((unsigned short)(D_8009D2A4[0] - 3) < 0x180) {
        *arg0[1] = (short)D_8009D2A4[0] - 3;
        *(unsigned short *)(g_TaskNodePool + 8) &= 0xFFDF;
        return 1;
    }

    if ((short)D_8009D2A4[0] == -1) {
        *arg0[1] = -1;
        *(unsigned short *)(g_TaskNodePool + 8) &= 0xFFDF;
        return 1;
    }

    Menu_OpenEquipSlotSwap(*arg0[0]);
    g_SceneDataTable0 -= 0x10;
    *(int *)(g_TaskNodePool + 0x10) = 1;
    return 0;
}
