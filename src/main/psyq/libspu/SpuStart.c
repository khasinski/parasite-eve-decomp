/* CC1_VERSION: 2.8.1 */
#include "pe1/psyq_spu_internal.h"

extern void EnterCriticalSection(void);
extern void ExitCriticalSection(void);
extern s32 OpenEvent(s32, s32, s32, void *);
extern void EnableEvent(s32);
extern void _SpuDataCallback(SpuCallback callback);
extern void _spu_FiDMA(void);

void SpuStart(void) {
    if (_spu_isCalled == 0) {
        _spu_isCalled = 1;
        EnterCriticalSection();
        _SpuDataCallback((SpuCallback)_spu_FiDMA);
        _spu_EVdma = OpenEvent(0xF0000009, 0x20, 0x2000, 0);
        EnableEvent(_spu_EVdma);
        ExitCriticalSection();
    }
}
