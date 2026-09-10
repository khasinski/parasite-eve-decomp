/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -fcall-used-$1 -fno-schedule-insns2 */
#include "pe1/psyq_cd.h"

void StClearRing(void);

void StSetRing(u32 *ring_addr, u32 ring_size) {
    StRingAddr = (StHEADER *)ring_addr;
    StRingSize = ring_size;
    StClearRing();
}
