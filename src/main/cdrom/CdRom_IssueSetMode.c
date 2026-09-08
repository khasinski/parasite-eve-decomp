#include "pe1/psyq_cd.h"

int CdRom_SendCmd(int arg0);

int func_8007FCBC(int arg0) {
    volatile int *ptr;
    ptr = &g_CdRomCmdTimeout;
    if (*ptr > 0) {
        return 0;
    }
    asm volatile("" : "=r"(ptr) : "0"(ptr));
    ptr[-10] = 0x20;
    return CdRom_SendCmd(arg0 & 0xFF);
}
