/* MASPSX_FLAGS: --stack-return-delay */

#include "include_asm.h"

/* Function body emitted as asm() (PSYQ GCC 2.7.2 register
 * allocation / scheduling diverges from the ROM pervasively).
 * Wrapped via maspsx's INCLUDE_ASM hack so the body is passed
 * through verbatim; bare global loads gp-rel via the .extern size
 * hints + `as -G8`. See tools/maspsx/PE1_IMPROVEMENTS.md. */
void __maspsx_include_asm_hack_DS_newmedia(void);

/* Function body emitted as asm() (PSYQ GCC 2.7.2 register
 * allocation / scheduling diverges from the ROM pervasively).
 * Wrapped via maspsx's INCLUDE_ASM hack so the body is passed
 * through verbatim; bare global loads gp-rel via the .extern size
 * hints + `as -G8`. See tools/maspsx/PE1_IMPROVEMENTS.md. */
INCLUDE_ASM("asm/USA/main/nonmatchings/psyq/misc34", DS_newmedia);

int strcmp(char *a, char *b);

extern int D_800A3CB4[];
extern char D_800A3CBC[];

int DS_searchdir(int dev, char *name) {
    register int saved_dev asm("$19");
    register char *saved_name asm("$20");
    register int i asm("$16");
    register int offset asm("$17");
    register char *entry_name asm("$18");
    register int value asm("$2");

    saved_dev = dev;
    saved_name = name;
    i = 0;
    entry_name = D_800A3CBC;
    offset = 0;
    do {
        asm volatile(
            "lui %0,%%hi(D_800A3CB4)\n\t"
            "addu %0,%0,%1\n\t"
            "lw %0,%%lo(D_800A3CB4)(%0)"
            : "=r"(value)
            : "r"(offset));

        if (value == 0) {
            return -1;
        }
        if (value == saved_dev) {
            if (strcmp(saved_name, entry_name) == 0) {
                return i + 1;
            }
            entry_name += 0x2C;
        } else {
            entry_name += 0x2C;
        }
        i++;
        offset += 0x2C;
    } while (i < 0x80);

    return -1;
}

INCLUDE_ASM("asm/USA/main/nonmatchings/psyq/misc34", DS_cachefile);
