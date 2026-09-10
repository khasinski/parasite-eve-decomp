/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */
#include "pe1/psyq_spu_internal.h"
extern char D_80011C4C[], D_80011C6C[], D_80011C80[];
int printf(const char *, ...);

void _spu_FwriteByIO(void *address, u32 size) {
    u16 *source = address;
    u16 initial_status;
    u32 timer;
    int count, i;
    {
        SpuRegs *spu = _spu_RXX;
        initial_status = spu->transfer_status & 0x7FF;
        spu->trans_addr = g_SpuTransferAddr;
    }
    _spu_Fw1ts();
    while (size) {
        count = size > 64 ? 64 : size;
        for (i = 0; i < count; i += 2) _spu_RXX->transfer_fifo = *source++;
        {
            SpuRegs *spu = _spu_RXX;
            u16 control = spu->spucnt;
            /* Single control write in the following wait call's delay slot. */
            *(u16 *)&spu->spucnt = (control & ~0x30) | 0x10;
        }
        _spu_Fw1ts();
        timer = 0;
        while (_spu_RXX->transfer_status & 0x400) {
            if (++timer > 0xF00u) {
                printf(D_80011C4C, D_80011C6C);
                break;
            }
        }
        _spu_Fw1ts();
        size -= count;
        _spu_Fw1ts();
    }
    {
        SpuRegs *spu = _spu_RXX;
        u16 control = spu->spucnt;
        spu->spucnt = control & ~0x30;
    }
    timer = 0;
    while ((_spu_RXX->transfer_status & 0x7FF) != initial_status) {
        if (++timer > 0xF00u) {
            printf(D_80011C4C, D_80011C80);
            break;
        }
    }
}
