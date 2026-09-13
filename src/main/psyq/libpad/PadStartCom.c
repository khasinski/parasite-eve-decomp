/* ASSEMBLER: GNU */
/* SDK _padStartCom (PADMAIN.OBJ). Constraints are tracked in crutch debt.
 * See proposals/PsyqPadFirstfile for provenance and full-byte validation. */
#include "pe1/psyq_pad_main.h"
void EnterCriticalSection(void);
void ExitCriticalSection(void);
void SysDeqIntRP(int, void *);
void SysEnqIntRP(int, void *);
void ChangeClearRCnt(int, int);
void MemCard_StartCounterIrq(void) {
    D_8009B75C = 0;
    EnterCriticalSection();
    SysDeqIntRP(2, D_800A5AB0);
    SysEnqIntRP(2, D_800A5AB0);
    D_8009B784->status = -2;
    D_8009B784->mask |= 1;
    ChangeClearRCnt(3, 0);
    ExitCriticalSection();
    D_8009B728(D_8009B758);
    D_8009B728(D_8009B758 + 1);
    {
        register unsigned *clear = D_800A5AC0;
        asm("" : "=r"(clear) : "0"(clear));
        clear[1] = 0;
        clear[0] = 0;
        asm("" ::: "memory");
    }
    D_8009B75C = 1;
    return;
}
