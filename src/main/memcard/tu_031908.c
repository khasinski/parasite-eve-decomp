/* CC1_FLAGS: -g3 */

/* Function body emitted as asm() (PSYQ GCC 2.7.2 register
 * allocation / scheduling diverges from the ROM pervasively).
 * Wrapped via maspsx's INCLUDE_ASM hack so the body is passed
 * through verbatim; bare global loads gp-rel via the .extern size
 * hints + `as -G8`. See tools/maspsx/PE1_IMPROVEMENTS.md. */

#include "include_asm.h"
void __maspsx_include_asm_hack_MemCard_UpdateSaveState(void);

/* Function body emitted as asm() (PSYQ GCC 2.7.2 register
 * allocation / scheduling diverges from the ROM pervasively).
 * Wrapped via maspsx's INCLUDE_ASM hack so the body is passed
 * through verbatim; bare global loads gp-rel via the .extern size
 * hints + `as -G8`. See tools/maspsx/PE1_IMPROVEMENTS.md. */
INCLUDE_ASM("asm/USA/main/nonmatchings/memcard/tu_031908", MemCard_UpdateSaveState);

INCLUDE_ASM("asm/USA/main/nonmatchings/memcard/tu_031908", Save_StartWriteSlot);
