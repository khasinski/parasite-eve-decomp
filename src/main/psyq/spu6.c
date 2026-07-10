/* MASPSX_FLAGS: --stack-return-delay */
#include "include_asm.h"

typedef unsigned short u16;
typedef unsigned int u32;

typedef struct SpuRegs {
    char pad0[0x1A6];
    volatile u16 trans_addr;
    char pad1[2];
    volatile u16 spucnt;
} SpuRegs;

extern SpuRegs *_spu_RXX;
extern volatile u32 *g_SpuDmaMadrPtr;
extern volatile u32 *g_SpuDmaBcrPtr;
extern volatile u32 *g_SpuDmaChcrPtr;
extern int g_SpuDmaDirection;

void _spu_Fw1ts(void);
void _spu_FsetDelayR(void);
void _spu_FsetDelayW(void);

extern int D_8009B418;
extern unsigned short g_SpuTransferAddr;
extern int _spu_mem_mode_plus;
extern int g_SpuDmaMadr;
extern int g_SpuDmaBcr;

int _spu_t(int cmd, ...);
void _spu_FwriteByIO(int addr, int size);

void _spu_Fr_(int madr, u16 trans_addr, int bcr) {
    int shiftedBcr;

    _spu_RXX->trans_addr = trans_addr;
    _spu_Fw1ts();
    _spu_RXX->spucnt |= 0x30;
    _spu_Fw1ts();
    shiftedBcr = bcr << 16;
    _spu_FsetDelayR();
    *g_SpuDmaMadrPtr = madr;
    *g_SpuDmaBcrPtr = shiftedBcr | 0x10;
    g_SpuDmaDirection = 1;
    *g_SpuDmaChcrPtr = 0x01000200;
}

int _spu_t(int cmd, ...) {
    int *args;
    int ret;
    unsigned int i;
    register int value asm("$4");
    int mode;
    int size;
    u32 chcr;

    args = &cmd + 1;
    ret = 0;

    if (cmd == 1) {
        goto cmd_1;
    }
    if (cmd >= 2) {
        goto ge_2;
    }
    if (cmd == 0) {
        goto cmd_0;
    }
    goto out;

ge_2:
    if (cmd == 2) {
        goto cmd_2;
    }
    if (cmd == 3) {
        goto cmd_3;
    }
    goto out;

cmd_2:
    value = *args++;
    value >>= _spu_mem_mode_plus;
    g_SpuTransferAddr = value;
    _spu_RXX->trans_addr = value;
    goto out;

cmd_1:
    g_SpuDmaDirection = 0;
    i = 0;
    if (_spu_RXX->trans_addr != g_SpuTransferAddr) {
        do {
            i++;
            if (i >= 0xF01) {
                return -2;
            }
        } while (_spu_RXX->trans_addr != g_SpuTransferAddr);
    }
    _spu_RXX->spucnt = (_spu_RXX->spucnt & 0xFFCF) | 0x20;
    goto out;

cmd_0:
    g_SpuDmaDirection = 1;
    i = 0;
    if (_spu_RXX->trans_addr != g_SpuTransferAddr) {
        do {
            i++;
            if (i >= 0xF01) {
                return -2;
            }
        } while (_spu_RXX->trans_addr != g_SpuTransferAddr);
    }
    _spu_RXX->spucnt |= 0x30;
    goto out;

cmd_3:
    mode = 0x20;
    if (g_SpuDmaDirection == 1) {
        mode = 0x30;
    }

    i = 0;
    if ((_spu_RXX->spucnt & 0x30) != (u32)mode) {
        do {
            i++;
            if (i >= 0xF01) {
                return -2;
            }
        } while ((_spu_RXX->spucnt & 0x30) != (u32)mode);
    }

    if (g_SpuDmaDirection == 1) {
        _spu_FsetDelayR();
    } else {
        _spu_FsetDelayW();
    }

    value = *args++;
    g_SpuDmaMadr = value;
    size = *args++;
    g_SpuDmaBcr = (size >> 6) + ((size & 0x3F) != 0);
    *g_SpuDmaMadrPtr = g_SpuDmaMadr;
    *g_SpuDmaBcrPtr = (g_SpuDmaBcr << 16) | 0x10;
    chcr = 0x01000201;
    if (g_SpuDmaDirection == 1) {
        chcr = 0x01000200;
    }
    *g_SpuDmaChcrPtr = chcr;

out:
    return ret;
}

int _spu_Fw(int addr, int size) {
    if (D_8009B418 == 0) {
        _spu_t(2, g_SpuTransferAddr << _spu_mem_mode_plus);
        _spu_t(1);
        _spu_t(3, addr, size);
    } else {
        _spu_FwriteByIO(addr, size);
    }

    return size;
}
