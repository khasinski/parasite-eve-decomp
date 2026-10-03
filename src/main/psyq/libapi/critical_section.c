#include "pe1/psyq_bios.h"

int EnterCriticalSection(void) {
    register int service asm("$4") = 1;
    int result;
    PSYQ_BIOS_SYSCALL(result, service);
    return result;
}
void ExitCriticalSection(void) {
    register int service asm("$4") = 2;
    int result;
    PSYQ_BIOS_SYSCALL(result, service);
}
