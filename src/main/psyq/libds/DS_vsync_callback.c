/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -fcall-used-$1 */

#include "pe1/psyq_ds.h"

extern unsigned int D_800A36A0;
extern unsigned int D_800A36A4;
extern unsigned int D_800A36A8;
extern unsigned int D_800A36AC;

void DS_vsync_callback(unsigned int value) {
    D_800A36A0 = value;
}

void DS_sync_callback(unsigned int value) {
    D_800A36A4 = value;
}

void DS_ready_callback(unsigned int value) {
    D_800A36A8 = value;
}

void DS_start_callback(unsigned int value) {
    D_800A36AC = value;
}
