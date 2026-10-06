#include "pe1/menu_inventory.h"
extern char *g_CurrentEntity;

int Inv_AddItem(int arg0);

int Inv_CountByValue(int arg0);

int Task_ScaleAnimValue(int **arg0) {
    int value = *arg0[0];
    int scale = *(short *)(g_CurrentEntity + 0x224) * 2;
    char *dst = *(char **)(g_CurrentEntity + 0x1B4);

    *(short *)(dst + 0x14) = (scale * value) >> 16;
    return 1;
}

int Task_ClampMenuRange(int **arg0) {
    Menu_ClampRange(*arg0[0]);
    return 1;
}

int Task_GiveItem(int **arg0) {
    *arg0[1] = Inv_AddItem(*arg0[0]);
    return 1;
}

int Task_CountItemsByValue(int **arg0) {
    *arg0[1] = Inv_CountByValue(*arg0[0]);
    return 1;
}
