/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */

#include "common.h"
#include "pe1/gpu_callbacks.h"

extern GpuCallbacks *D_80095744;

int DrawPrim(void *primitive) {
    u8 *bytes = primitive;
    int word_count = bytes[3];

    D_80095744->callback3C(0);
    return D_80095744->cb14(bytes + 4, word_count);
}
