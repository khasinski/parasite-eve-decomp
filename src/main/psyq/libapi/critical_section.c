#include "pe1/psyq_bios.h"

int EnterCriticalSection(void) {
    register int service asm("$4") = 1;
    register int result asm("$2");
    PSYQ_BIOS_SYSCALL(result, service);
    return result;
}
void ExitCriticalSection(void) {
    register int service asm("$4") = 2;
    register int result asm("$2");
    PSYQ_BIOS_SYSCALL(result, service);
}
