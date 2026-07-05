#include "include_asm.h"

void MenuWidget_NavScrollTo(int arg);

int Menu_MemCardProgressInputHandler(void) {
    return 1;
}

INCLUDE_ASM("asm/USA/main/nonmatchings/menu/tu_03E29C", Menu_StepItemGrid2);

void Menu_NavToSaveConfirmDialog(void) {
    MenuWidget_NavScrollTo(0x2A);
}
