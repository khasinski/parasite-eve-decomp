/* CC1_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */
/* NOTE: cc.sh does not yet whitelist -mno-split-addresses; until it does,
 * build with PE_CC1_EXTRA_FLAGS=-mno-split-addresses. */
#include "common.h"

typedef void *va_list;

#define VA_ROUNDED_SIZE(type) \
    (((sizeof(type) + sizeof(long) - 1) / sizeof(long)) * sizeof(long))
#define va_start(list, last) \
    (list = (void *)((u8 *)&(last) + VA_ROUNDED_SIZE(last)))
#define va_arg(list, type) \
    (list = (void *)((u8 *)list + VA_ROUNDED_SIZE(type)), \
     *(type *)((u8 *)list - VA_ROUNDED_SIZE(type)))

extern volatile u16 *D_8009B3FC;
extern u16 D_8009B414;
extern int D_8009B424;
extern int D_8009B44C;
extern int D_8009B418;
extern volatile u32 *D_8009B400;
extern volatile u32 *D_8009B404;
extern volatile u32 *D_8009B408;
extern u32 D_8009B450;
extern u32 D_8009B454;

void _spu_Fw1ts(void);
void _spu_FsetDelayR(void);
void _spu_FsetDelayW(void);
void _spu_FwriteByIO(long addr, long size);
long _spu_t(long op, ...);

void _spu_Fr_(u32 addr, u16 start, u32 blocks) {
    D_8009B3FC[211] = start;
    _spu_Fw1ts();
    D_8009B3FC[213] |= 0x30;
    _spu_Fw1ts();
    _spu_FsetDelayR();
    *D_8009B400 = addr;
    *D_8009B404 = (blocks << 16) | 0x10;
    D_8009B44C = 1;
    *D_8009B408 = 0x1000200;
}

long _spu_t(long op, ...) {
    long dmaControl;
    u32 i;
    va_list args;
    u32 count;
    u16 mode;

    va_start(args, op);

    switch (op) {
    case 2:
        count = va_arg(args, u32);
        D_8009B414 = count >> D_8009B424;
        D_8009B3FC[211] = D_8009B414;
        break;

    case 1:
        D_8009B44C = 0;
        i = 0;
        while (D_8009B3FC[211] != D_8009B414) {
            if (++i > 0xF00) {
                return -2;
            }
        }
        D_8009B3FC[213] = (D_8009B3FC[213] & ~0x30) | 0x20;
        break;

    case 0:
        D_8009B44C = 1;
        i = 0;
        while (D_8009B3FC[211] != D_8009B414) {
            if (++i > 0xF00) {
                return -2;
            }
        }
        D_8009B3FC[213] |= 0x30;
        break;

    case 3:
        if (D_8009B44C == 1) {
            mode = 0x30;
        } else {
            mode = 0x20;
        }

        i = 0;
        while ((D_8009B3FC[213] & 0x30) != mode) {
            if (++i > 0xF00) {
                return -2;
            }
        }

        if (D_8009B44C == 1) {
            _spu_FsetDelayR();
        } else {
            _spu_FsetDelayW();
        }

        count = va_arg(args, u32);
        D_8009B450 = count;
        count = va_arg(args, u32);
        D_8009B454 = (count >> 6) + ((count & 0x3F) ? 1 : 0);
        *D_8009B400 = D_8009B450;
        *D_8009B404 = (D_8009B454 << 16) | 0x10;

        if (D_8009B44C == 1) {
            dmaControl = 0x1000200;
        } else {
            dmaControl = 0x1000201;
        }
        *D_8009B408 = dmaControl;
        break;
    }

    return 0;
}

long _spu_Fw(long addr, long size) {
    if (D_8009B418 == 0) {
        _spu_t(2, D_8009B414 << D_8009B424);
        _spu_t(1);
        _spu_t(3, addr, size);
    } else {
        _spu_FwriteByIO(addr, size);
    }

    return size;
}
