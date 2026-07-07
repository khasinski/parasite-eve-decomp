/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: --use-comm-section -G8 */


#include "include_asm.h"
int g_MenuInputPollingPaused;

int g_MenuDrawPhase;

/* Function body emitted as asm() (PSYQ GCC 2.7.2 register
 * allocation / scheduling diverges from the ROM pervasively).
 * Wrapped via maspsx's INCLUDE_ASM hack so the body is passed
 * through verbatim; bare global loads gp-rel via the .extern size
 * hints + `as -G8`. See tools/maspsx/PE1_IMPROVEMENTS.md. */
void __maspsx_include_asm_hack_MenuInput_EnqueueStatusChanges(void);

/* Function body emitted as asm() (PSYQ GCC 2.7.2 register
 * allocation / scheduling diverges from the ROM pervasively).
 * Wrapped via maspsx's INCLUDE_ASM hack so the body is passed
 * through verbatim; bare global loads gp-rel via the .extern size
 * hints + `as -G8`. See tools/maspsx/PE1_IMPROVEMENTS.md. */
void __maspsx_include_asm_hack_MenuInput_DispatchQueuedEvents(void);

void MenuInput_SetPollingPaused(int arg0) {
    g_MenuInputPollingPaused = arg0;
}

int MenuInput_GetRepeatStep(void) {
    return g_MenuDrawPhase;
}

INCLUDE_ASM("asm/USA/main/nonmatchings/menu/menu_input", MenuInput_EnqueueStatusChanges);

INCLUDE_ASM("asm/USA/main/nonmatchings/menu/menu_input", MenuInput_DispatchQueuedEvents);
