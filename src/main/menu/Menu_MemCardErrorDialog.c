/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: --use-comm-section -G8 */
#include "pe1/menu_inventory.h"

unsigned int g_MenuMemCardConfirmPending;

int g_McDialogMode;

void Menu_CreateTwoLineDialog(int arg0, int arg1);

void Menu_SetMemCardConfirmPending(void) {
    g_MenuMemCardConfirmPending = 1;
}

void Menu_ShowMemCardErrorDialog(int arg0) {
    if (MenuWidget_FindByModeAndSelectedBase(1, 0x28) == 0) {
        Menu_CreateTwoLineDialog(arg0 + 0x47, g_McDialogMode + 0x42);
    }
}
