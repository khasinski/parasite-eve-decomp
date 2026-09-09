/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -fcall-used-$1 -fno-schedule-insns2 */
#include "common.h"

extern u32 *StRingAddr __asm__("D_800C0DC8");
extern u32 StRingSize __asm__("D_800C20C4");
void StClearRing(void);

void StSetRing(u32 *ring_addr, u32 ring_size) {
    StRingAddr = ring_addr;
    StRingSize = ring_size;
    StClearRing();
}
