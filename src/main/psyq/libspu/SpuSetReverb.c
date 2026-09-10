/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */
#include "common.h"
#include "pe1/psyq_spu_internal.h"
#include "pe1/akao/spu_common.h"

extern s32 _spu_rev_flag, _spu_rev_reserve_wa;
extern u32 _spu_rev_offsetaddr;
extern s32 _SpuIsInAllocateArea_(u32);

int SpuSetReverb(int on_off) {
    switch (on_off) {
    case 0:
        _spu_rev_flag = 0;
        _spu_RXX->spucnt &= ~0x80;
        break;
    case 1:
        if (_spu_rev_reserve_wa != 1 &&
            _SpuIsInAllocateArea_(_spu_rev_offsetaddr)) {
            _spu_rev_flag = 0;
            _spu_RXX->spucnt &= ~0x80;
        } else {
            _spu_rev_flag = on_off;
            _spu_RXX->spucnt |= 0x80;
        }
        break;
    }
    return _spu_rev_flag;
}
