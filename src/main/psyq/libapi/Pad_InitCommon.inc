#include "common.h"
#include "pe1/pad_internal.h"

void _remove_ChgclrPAD(void);
void EnterCriticalSection(void);
void _patch_pad(void);
void ExitCriticalSection(void);
void ChangeClearPAD(int mode);
int Sys_SetInterruptHandler(void);
void PAD_INIT_COMMON_CALL(void *buffer0, int size0, void *buffer1, int size1);

int PAD_INIT_COMMON_NAME(void *buffer0, int size0, void *buffer1, int size1) {
    _remove_ChgclrPAD();
    EnterCriticalSection();
    _patch_pad();
    ExitCriticalSection();
    ChangeClearPAD(0);
    Sys_SetInterruptHandler();
    PAD_INIT_COMMON_CALL(buffer0, size0, buffer1, size1);
    g_InitPadFlag = 1;
    PE1_COMPILER_MEMORY_BARRIER();
    return 1;
}

#undef PAD_INIT_COMMON_NAME
#undef PAD_INIT_COMMON_CALL
