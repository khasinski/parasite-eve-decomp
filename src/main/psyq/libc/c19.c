/* PSY-Q LIBC C19: setjmp. */
#include "pe1/psyq_bios.h"

/* BIOS A(13h) setjmp veneer; Sys_InitIntrManager calls it. */
PSYQ_BIOS_TRAMPOLINE(setjmp, 0xA0, 0x13);
