/* Function body emitted as asm() (PSYQ GCC 2.7.2 register
 * allocation / scheduling diverges from the ROM pervasively).
 * Wrapped via maspsx's INCLUDE_ASM hack so the body is passed
 * through verbatim; bare global loads gp-rel via the .extern size
 * hints + `as -G8`. See tools/maspsx/PE1_IMPROVEMENTS.md. */

#include "include_asm.h"
INCLUDE_ASM("asm/USA/main/nonmatchings/psyq/libcd/getintr", getintr);
