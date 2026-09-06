/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */
#include "common.h"
#include "pe1/akao/spu_common.h"

extern volatile u16 *_spu_RXX;
extern int g_SpuReverbEnabled, g_SpuReverbAllocationOverride;
extern unsigned int g_SpuReverbWorkArea;
extern int _SpuIsInAllocateArea_(unsigned int);

int SPU_ReadRegister(int enabled) {
    switch (enabled) {
    case 0:
        g_SpuReverbEnabled = 0;
        _spu_RXX[0xD5] &= ~0x80;
        break;
    case 1:
        if (g_SpuReverbAllocationOverride != 1 &&
            _SpuIsInAllocateArea_(g_SpuReverbWorkArea)) {
            g_SpuReverbEnabled = 0;
            _spu_RXX[0xD5] &= ~0x80;
        } else {
            g_SpuReverbEnabled = enabled;
            _spu_RXX[0xD5] |= 0x80;
        }
        break;
    }
    return g_SpuReverbEnabled;
}
