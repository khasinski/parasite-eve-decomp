/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: --use-comm-section -G8 */
#include "include_asm.h"

signed char g_PlayerMovementEnabled;

int g_TaskCollisionFlag;

signed char g_MenuEquipSlotIndex;

void Task_EnableMovement(void) {
    g_PlayerMovementEnabled = 1;
}

void Task_DisableMovement(void) {
    g_PlayerMovementEnabled = 0;
}

void Task_SetCollisionFlag(int arg0) {
    g_TaskCollisionFlag = arg0 != 0;
}

INCLUDE_ASM("asm/USA/main/nonmatchings/main/task3", Render_SetupColorTable);

int Menu_GetEquipSlotIndex(void) {
    return g_MenuEquipSlotIndex;
}
