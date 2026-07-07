/* CC1_FLAGS: -G8 */


#include "include_asm.h"
void *MenuWidget_FindByModeAndSelectedBase(int arg0, int arg1);
void MenuWidget_DestroyNode(void *arg0);

/* Function body emitted as asm() (PSYQ GCC 2.7.2 register
 * allocation / scheduling diverges from the ROM pervasively).
 * Wrapped via maspsx's INCLUDE_ASM hack so the body is passed
 * through verbatim; bare global loads gp-rel via the .extern size
 * hints + `as -G8`. See tools/maspsx/PE1_IMPROVEMENTS.md. */
void __maspsx_include_asm_hack_func_8004C608(void);

void Menu_CloseContextHelpPanel(void) {
    MenuWidget_DestroyNode(MenuWidget_FindByModeAndSelectedBase(1, 0x13));
}

INCLUDE_ASM("asm/USA/main/nonmatchings/menu/misc24", Menu_DrawContextHelpText);
