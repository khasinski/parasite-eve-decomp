#include "common.h"
#include "pe1/psyq_spu_internal.h"
#include "pe1/akao/spu_common.h"

void _spu_setReverbAttr(SpuReverbRegisterAttrs *attr) {
    u32 mask = attr->mask;
    u32 setAll = (mask < 1);

    if (setAll || (mask & 0x00000001)) {
        _spu_RXX->reverb[0] = attr->regs[0];
    }
    if (setAll || (mask & 0x00000002)) {
        _spu_RXX->reverb[1] = attr->regs[1];
    }
    if (setAll || (mask & 0x00000004)) {
        _spu_RXX->reverb[2] = attr->regs[2];
    }
    if (setAll || (mask & 0x00000008)) {
        _spu_RXX->reverb[3] = attr->regs[3];
    }
    if (setAll || (mask & 0x00000010)) {
        _spu_RXX->reverb[4] = attr->regs[4];
    }
    if (setAll || (mask & 0x00000020)) {
        _spu_RXX->reverb[5] = attr->regs[5];
    }
    if (setAll || (mask & 0x00000040)) {
        _spu_RXX->reverb[6] = attr->regs[6];
    }
    if (setAll || (mask & 0x00000080)) {
        _spu_RXX->reverb[7] = attr->regs[7];
    }
    if (setAll || (mask & 0x00000100)) {
        _spu_RXX->reverb[8] = attr->regs[8];
    }
    if (setAll || (mask & 0x00000200)) {
        _spu_RXX->reverb[9] = attr->regs[9];
    }
    if (setAll || (mask & 0x00000400)) {
        _spu_RXX->reverb[10] = attr->regs[10];
    }
    if (setAll || (mask & 0x00000800)) {
        _spu_RXX->reverb[11] = attr->regs[11];
    }
    if (setAll || (mask & 0x00001000)) {
        _spu_RXX->reverb[12] = attr->regs[12];
    }
    if (setAll || (mask & 0x00002000)) {
        _spu_RXX->reverb[13] = attr->regs[13];
    }
    if (setAll || (mask & 0x00004000)) {
        _spu_RXX->reverb[14] = attr->regs[14];
    }
    if (setAll || (mask & 0x00008000)) {
        _spu_RXX->reverb[15] = attr->regs[15];
    }
    if (setAll || (mask & 0x00010000)) {
        _spu_RXX->reverb[16] = attr->regs[16];
    }
    if (setAll || (mask & 0x00020000)) {
        _spu_RXX->reverb[17] = attr->regs[17];
    }
    if (setAll || (mask & 0x00040000)) {
        _spu_RXX->reverb[18] = attr->regs[18];
    }
    if (setAll || (mask & 0x00080000)) {
        _spu_RXX->reverb[19] = attr->regs[19];
    }
    if (setAll || (mask & 0x00100000)) {
        _spu_RXX->reverb[20] = attr->regs[20];
    }
    if (setAll || (mask & 0x00200000)) {
        _spu_RXX->reverb[21] = attr->regs[21];
    }
    if (setAll || (mask & 0x00400000)) {
        _spu_RXX->reverb[22] = attr->regs[22];
    }
    if (setAll || (mask & 0x00800000)) {
        _spu_RXX->reverb[23] = attr->regs[23];
    }
    if (setAll || (mask & 0x01000000)) {
        _spu_RXX->reverb[24] = attr->regs[24];
    }
    if (setAll || (mask & 0x02000000)) {
        _spu_RXX->reverb[25] = attr->regs[25];
    }
    if (setAll || (mask & 0x04000000)) {
        _spu_RXX->reverb[26] = attr->regs[26];
    }
    if (setAll || (mask & 0x08000000)) {
        _spu_RXX->reverb[27] = attr->regs[27];
    }
    if (setAll || (mask & 0x10000000)) {
        _spu_RXX->reverb[28] = attr->regs[28];
    }
    if (setAll || (mask & 0x20000000)) {
        _spu_RXX->reverb[29] = attr->regs[29];
    }
    if (setAll || (mask & 0x40000000)) {
        _spu_RXX->reverb[30] = attr->regs[30];
    }
    if (setAll || (mask & 0x80000000)) {
        _spu_RXX->reverb[31] = attr->regs[31];
    }
}
