#include "pe1/psyq_spu_internal.h"
extern char D_8001208C[];
extern char D_8001209C[];
extern char D_800120AC[];
int printf(const char *, ...);
long SpuSetIRQ(long mode) {
    u32 timer;
    if (mode == 0 || mode == 3) {
        _spu_RXX->spucnt &= ~0x40;
        timer = 0;
        while (_spu_RXX->spucnt & 0x40) {
            if (++timer > 0xF00u) {
                printf(D_8001208C, D_8001209C);
                return -1;
            }
        }
    }
    if (mode == 1 || mode == 3) {
        _spu_RXX->spucnt |= 0x40;
        timer = 0;
        while (!(_spu_RXX->spucnt & 0x40)) {
            if (++timer > 0xF00u) {
                printf(D_8001208C, D_800120AC);
                return -1;
            }
        }
    }
    return mode;
}
