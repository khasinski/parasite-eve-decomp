#include "pe1/psyq_spu_internal.h"

extern int EnterCriticalSection(void);
extern void ExitCriticalSection(void);
extern s32 CloseEvent(s32 event);
extern s32 DisableEvent(s32 event);


void SpuQuit(void) {
    if (_spu_isCalled == 1) {
        _spu_isCalled = 0;
        EnterCriticalSection();
        _spu_transferCallback = 0;
        _spu_IRQCallback = 0;
        _SpuDataCallback(0);
        CloseEvent(_spu_EVdma);
        DisableEvent(_spu_EVdma);
        ExitCriticalSection();
    }
}
