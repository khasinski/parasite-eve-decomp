/* ASSEMBLER: GNU */
/* Psy-Q LIBCARD INIT.OBJ: InitCARD, StartCARD, StopCARD. */
#include "pe1/pad_internal.h"
#include "pe1/psyq_bios.h"
#include "pe1/psyq_card.h"

void InitCARD(int padEnable) {
    ChangeClearPAD(0);
    EnterCriticalSection();
    if (ReadInitPadFlag() == 0) {
        padEnable = 0;
    }
    InitCARD2(padEnable);
    _copy_memcard_patch();
    _patch_card();
    _patch_card2();
    ExitCriticalSection();
}

long StartCARD(void) {
    EnterCriticalSection();
    StartCARD2();
    ChangeClearPAD(0);
    ExitCriticalSection();
}

long StopCARD(void) {
    StopCARD2();
    _ExitCard();
}
