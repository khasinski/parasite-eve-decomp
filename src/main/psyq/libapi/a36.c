/* PSY-Q LIBAPI A36: EnterCriticalSection. */
#include "pe1/psyq_bios.h"

int EnterCriticalSection(void) {
    register int service asm("$4") = 1;
    int result;
    PSYQ_BIOS_SYSCALL(result, service);
    return result;
}
