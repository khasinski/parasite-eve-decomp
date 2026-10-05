/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */
#include "common.h"

extern int D_800A34B0;
extern void (*D_800A34B4[2])(void);
extern int D_800A34BC;

void EnterCriticalSection(void);
void ExitCriticalSection(void);
void SysDeqIntRP(int index, void *queue);
void SysEnqIntRP(int index, void *queue);
void func_8007E0F8(void);
void func_8007E160(void);

int Sys_SetInterruptHandler(void) {
    void *queue;
    void (**handlers)(void);
    int result;
    EnterCriticalSection();
    handlers = D_800A34B4;
    queue = (char *)handlers - 4;
    handlers[0] = func_8007E0F8;
    handlers[1] = func_8007E160;
    D_800A34B0 = 0;
    D_800A34BC = 0;
    SysDeqIntRP(1, queue);
    SysEnqIntRP(1, queue);
    ExitCriticalSection();
    result = 1;
    PE1_COMPILER_LAUNDER(result);
    return result;
}
