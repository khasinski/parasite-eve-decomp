/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: --use-comm-section -G8 */
#include "include_asm.h"

signed char D_8009CE94;

short D_8009CE98;
short D_8009CE9A;
short D_8009CE9C;
short D_8009CE9E;

void Menu_SetEquipSlotIndex(int arg0) {
    D_8009CE94 = arg0;
}

INCLUDE_ASM("asm/USA/main/nonmatchings/menu/tu_0279A4", Render_SetupFogLayer);

void Menu_SetTextCursorRect(int arg0, int arg1, int arg2, int arg3) {
    D_8009CE98 = arg0;
    D_8009CE9A = arg1;
    D_8009CE9C = arg2;
    D_8009CE9E = arg3;
}
