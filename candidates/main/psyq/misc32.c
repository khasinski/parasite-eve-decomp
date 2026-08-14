/* CC1_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */

typedef signed int s32;
typedef unsigned int u32;
typedef unsigned short u16;
typedef void (*SpuCallback)(void);

typedef struct SpuRegs {
    /* 0x000 */ unsigned char pad_000[0x1A6];
    /* 0x1A6 */ volatile u16 transfer_addr;
    /* 0x1A8 */ volatile u16 transfer_fifo;
    /* 0x1AA */ volatile u16 spucnt;
    /* 0x1AC */ unsigned char pad_1AC[2];
    /* 0x1AE */ volatile u16 transfer_status;
} SpuRegs;

extern SpuRegs *_spu_RXX;
extern u16 g_SpuTransferAddr;
extern s32 g_SpuDmaDirection;
extern SpuCallback volatile _spu_transferCallback;

void _spu_Fw1ts(void);
int printf(const char *fmt, ...);
void DeliverEvent(unsigned int ev, int spec);

extern char D_80011C4C[]; /* "SPU:T/O [%s]\n" */
extern char D_80011C6C[]; /* "wait (wrdy H -> L)" */
extern char D_80011C80[]; /* "wait (dmaf clear/W)" */

void _spu_FwriteByIO(u16 *addr, u32 size) {
    u16 *p = addr;
    u16 stat0;
    u16 saved;
    u16 cnt;
    long chunk;
    long k;
    u32 i;
    s32 cur;

    stat0 = _spu_RXX->transfer_status;
    _spu_RXX->transfer_addr = g_SpuTransferAddr;
    saved = stat0 & 0x7FF;
    _spu_Fw1ts();

    while (size != 0) {
        chunk = (size > 0x40) ? 0x40 : size;
        for (k = 0; k < chunk; k += 2) {
            _spu_RXX->transfer_fifo = *p++;
        }
        cnt = _spu_RXX->spucnt;
        *(u16 *)&_spu_RXX->spucnt = (cnt & 0xFFCF) | 0x10;
        _spu_Fw1ts();
        i = 0;
        while (_spu_RXX->transfer_status & 0x400) {
            i++;
            if (i > 0xF00) {
                printf(D_80011C4C, D_80011C6C);
                break;
            }
        }
        _spu_Fw1ts();
        size -= chunk;
        _spu_Fw1ts();
    }

    cnt = _spu_RXX->spucnt;
    _spu_RXX->spucnt = cnt & 0xFFCF;
    i = 0;
    cur = _spu_RXX->transfer_status & 0x7FF;
    while (cur != saved) {
        i++;
        if (i > 0xF00) {
            printf(D_80011C4C, D_80011C80);
            return;
        }
        cur = _spu_RXX->transfer_status & 0x7FF;
    }
}

void _spu_FiDMA(void) {
    u32 i;

    if (g_SpuDmaDirection == 0) {
        _spu_Fw1ts();
    }

    _spu_RXX->spucnt &= 0xFFCF;
    i = 0;
    while (_spu_RXX->spucnt & 0x30) {
        i++;
        if (i > 0xF00) {
            break;
        }
    }

    if (_spu_transferCallback) {
        _spu_transferCallback();
    } else {
        DeliverEvent(0xF0000009, 0x20);
    }
}
