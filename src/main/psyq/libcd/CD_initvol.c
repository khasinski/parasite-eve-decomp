/* MASPSX_FLAGS: --stack-return-delay */

typedef unsigned char u8;
typedef unsigned short u16;

extern u8 *g_CdRegIndexBase;
extern u8 *g_CdRegPort1;
extern u8 *g_CdRegDataWrite;
extern u8 *g_CdRegResponse;
extern u16 *volatile D_8009B290;

int CD_initvol(void) {
    u8 packet[4];
    u16 *regs = D_8009B290;
    int value;
    int ret = 0;

    if (regs[0x1B8 / 2] != 0) {
        goto set_default;
    }
    if (regs[0x1BA / 2] != 0) {
        value = 0x3FFF;
        goto store_common;
    }
    value = 0x3FFF;
    regs[0x180 / 2] = value;
    regs[0x182 / 2] = value;
    regs = D_8009B290;

set_default:
    value = 0x3FFF;

store_common:
    regs[0x1B0 / 2] = value;
    regs[0x1B2 / 2] = value;
    regs[0x1AA / 2] = 0xC001;

    packet[2] = 0x80;
    packet[0] = 0x80;
    packet[3] = 0;
    packet[1] = 0;

    *g_CdRegIndexBase = 2;
    *g_CdRegDataWrite = packet[0];
    *g_CdRegResponse = packet[1];
    *g_CdRegIndexBase = 3;
    *g_CdRegPort1 = packet[2];
    *g_CdRegDataWrite = packet[3];
    *g_CdRegResponse = 0x20;

    return ret;
}
