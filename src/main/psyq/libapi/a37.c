/* PSY-Q LIBAPI A37: ExitCriticalSection. */
#include "pe1/psyq_bios.h"

void ExitCriticalSection(void) {
    register int service asm("$4") = 2;
    int result;
    PSYQ_BIOS_SYSCALL(result, service);
}
