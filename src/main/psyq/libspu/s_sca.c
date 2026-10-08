/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */
/* PSY-Q LIBSPU S_SCA: SpuSetCommonAttr. */
#include "common.h"
#include "pe1/psyq_spu_internal.h"

void SpuSetCommonAttr(SpuCommonAttr *attr) {
    /* Read raw volume bits through the corresponding unsigned halfword view. */
    u16 left = 0;
    register u16 right asm("$8");
    u32 mask;
    int all;
    u32 mode;

    /* Keep the initial zero values independent and before the mask read. */
    asm volatile("" : "+r"(left));
    mask = attr->mask;
    all = mask == 0;
    right = 0;
    if (all || (mask & 1)) {
        if (all || (mask & 4)) {
            switch (attr->mvolmode.left) {
            case 1: mode = 0x8000; break;
            case 2: mode = 0x9000; break;
            case 3: mode = 0xA000; break;
            case 4: mode = 0xB000; break;
            case 5: mode = 0xC000; break;
            case 6: mode = 0xD000; break;
            case 7: mode = 0xE000; break;
            case 0:
            default: left = (*(u16 *)&attr->mvol.left); mode = 0; break;
            }
        } else { left = (*(u16 *)&attr->mvol.left); mode = 0; }
        if (mode) {
            int value = (s16)(*(u16 *)&attr->mvol.left);
            left = value > 0x7F ? 0x7F : value < 0 ? 0 : (*(u16 *)&attr->mvol.left);
        }
        _spu_RXX->master_volume_left = (left & 0x7FFF) | mode;
    }
    if (all || (mask & 2)) {
        if (all || (mask & 8)) {
            switch (attr->mvolmode.right) {
            case 1: mode = 0x8000; break;
            case 2: mode = 0x9000; break;
            case 3: mode = 0xA000; break;
            case 4: mode = 0xB000; break;
            case 5: mode = 0xC000; break;
            case 6: mode = 0xD000; break;
            case 7: mode = 0xE000; break;
            case 0:
            default: right = (*(u16 *)&attr->mvol.right); mode = 0; break;
            }
        } else { right = (*(u16 *)&attr->mvol.right); mode = 0; }
        if (mode) {
            right = (s16)(*(u16 *)&attr->mvol.right) > 0x7F ? 0x7F : (s16)(*(u16 *)&attr->mvol.right) < 0 ? 0 : (*(u16 *)&attr->mvol.right);
        }
        _spu_RXX->master_volume_right = (right & 0x7FFF) | mode;
    }
    if (all || (mask & 0x40)) _spu_RXX->cd_volume_left = attr->cd.volume.left;
    if (all || (mask & 0x80)) _spu_RXX->cd_volume_right = attr->cd.volume.right;
    if (all || (mask & 0x400)) _spu_RXX->external_volume_left = attr->ext.volume.left;
    if (all || (mask & 0x800)) _spu_RXX->external_volume_right = attr->ext.volume.right;
    if (all || (mask & 0x100)) {
        if (!attr->cd.reverb) _spu_RXX->spucnt &= ~4;
        else _spu_RXX->spucnt |= 4;
    }
    if (all || (mask & 0x200)) {
        if (!attr->cd.mix) _spu_RXX->spucnt &= ~1;
        else _spu_RXX->spucnt |= 1;
    }
    if (all || (mask & 0x1000)) {
        if (!attr->ext.reverb) _spu_RXX->spucnt &= ~8;
        else _spu_RXX->spucnt |= 8;
    }
    if (all || (mask & 0x2000)) {
        if (!attr->ext.mix) _spu_RXX->spucnt &= ~2;
        else _spu_RXX->spucnt |= 2;
    }
    /* Preserve the right-volume temporary and the common return block. */
    asm volatile("" : : "r"(right));
}
