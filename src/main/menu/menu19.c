#include "include_asm.h"

void MenuWidget_NavScrollTo(int selected_base);

int Menu_MemCardProgressInputHandler(void) {
    return 1;
}

INCLUDE_ASM("asm/USA/main/nonmatchings/menu/menu19", Menu_StepItemGrid2);

void Menu_NavToSaveConfirmDialog(void) {
    MenuWidget_NavScrollTo(0x2A);
}
