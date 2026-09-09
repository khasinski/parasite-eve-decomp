/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */
#include "pe1/psyq_cd.h"
#include "pe1/psyq_spu_internal.h"

extern SpuRegs *volatile D_8009B290;

static inline int apply_volume(CdlATV *volume) {
    *g_CdRegIndexBase = 2;
    *g_CdRegDataWrite = volume->val0;
    *g_CdRegResponse = volume->val1;
    *g_CdRegIndexBase = 3;
    *g_CdRegPort1 = volume->val2;
    *g_CdRegDataWrite = volume->val3;
    *g_CdRegResponse = 0x20;
    return 0;
}

int CD_initvol(void) {
    SpuRegs *spu = D_8009B290;
    CdlATV volume;
    if (!spu->current_master_volume_left && !spu->current_master_volume_right) {
        spu->master_volume_left = 0x3FFF;
        spu->master_volume_right = 0x3FFF;
        spu = D_8009B290;
    }
    spu->cd_volume_left = 0x3FFF;
    spu->cd_volume_right = 0x3FFF;
    spu->spucnt = 0xC001;
    volume.val0 = volume.val2 = 0x80;
    volume.val1 = volume.val3 = 0;
    return apply_volume(&volume);
}
