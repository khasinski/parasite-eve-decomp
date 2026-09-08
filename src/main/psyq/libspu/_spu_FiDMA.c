#include "common.h"

typedef void (*SpuTransferCallback)(void);

#include "pe1/psyq_spu_internal.h"
extern s32 g_SpuDmaDirection;
extern SpuTransferCallback volatile _spu_transferCallback;
void _spu_Fw1ts(void);
int DeliverEvent(u32 event, u32 spec);

void _spu_FiDMA(void)
{
    u32 count;

    if (g_SpuDmaDirection == 0) {
        _spu_Fw1ts();
    }
    _spu_RXX->spucnt &= ~0x30;
    count = 0;
    while (_spu_RXX->spucnt & 0x30) {
        if (++count > 0xF00) {
            break;
        }
    }
    /* The poll counter is dead; do not compensate its speculative increment. */
    asm volatile("" : : : "$3");
    if (_spu_transferCallback != 0) {
        _spu_transferCallback();
    } else {
        DeliverEvent(0xF0000009, 0x20);
    }
}
