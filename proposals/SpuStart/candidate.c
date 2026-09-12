/* ASSEMBLER: GNU */
#include "pe1/psyq_spu_internal.h"

extern int EnterCriticalSection(void);
extern void ExitCriticalSection(void);
extern s32 OpenEvent(u32 event, s32 spec, s32 mode, s32 (*callback)(void));
extern s32 EnableEvent(s32 event);
extern void _spu_FiDMA(void);

void SpuStart(void) {
    if (_spu_isCalled == 0) {
        _spu_isCalled = 1;
        EnterCriticalSection();
        _SpuDataCallback(_spu_FiDMA);
        _spu_EVdma = OpenEvent(0xF0000009, 0x20, 0x2000, 0);
        EnableEvent(_spu_EVdma);
        ExitCriticalSection();
    }
}
