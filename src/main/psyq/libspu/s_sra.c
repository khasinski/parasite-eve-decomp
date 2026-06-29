/* CC1_FLAGS: -G8 */

typedef unsigned int u32;
typedef unsigned short u16;

extern unsigned char *_spu_RXX;

#define _spu_RXX _spu_RXX

typedef struct SpuReverbRegisterAttrs {
    u32 mask;
    u16 regs[32];
} SpuReverbRegisterAttrs;

void _spu_setReverbAttr(SpuReverbRegisterAttrs *attr) {
    u32 mask = attr->mask;
    u32 setAll = (mask < 1);

    if (setAll || (mask & 0x00000001)) {
        *(u16 *)(_spu_RXX + 0x1C0) = attr->regs[0];
    }
    if (setAll || (mask & 0x00000002)) {
        *(u16 *)(_spu_RXX + 0x1C2) = attr->regs[1];
    }
    if (setAll || (mask & 0x00000004)) {
        *(u16 *)(_spu_RXX + 0x1C4) = attr->regs[2];
    }
    if (setAll || (mask & 0x00000008)) {
        *(u16 *)(_spu_RXX + 0x1C6) = attr->regs[3];
    }
    if (setAll || (mask & 0x00000010)) {
        *(u16 *)(_spu_RXX + 0x1C8) = attr->regs[4];
    }
    if (setAll || (mask & 0x00000020)) {
        *(u16 *)(_spu_RXX + 0x1CA) = attr->regs[5];
    }
    if (setAll || (mask & 0x00000040)) {
        *(u16 *)(_spu_RXX + 0x1CC) = attr->regs[6];
    }
    if (setAll || (mask & 0x00000080)) {
        *(u16 *)(_spu_RXX + 0x1CE) = attr->regs[7];
    }
    if (setAll || (mask & 0x00000100)) {
        *(u16 *)(_spu_RXX + 0x1D0) = attr->regs[8];
    }
    if (setAll || (mask & 0x00000200)) {
        *(u16 *)(_spu_RXX + 0x1D2) = attr->regs[9];
    }
    if (setAll || (mask & 0x00000400)) {
        *(u16 *)(_spu_RXX + 0x1D4) = attr->regs[10];
    }
    if (setAll || (mask & 0x00000800)) {
        *(u16 *)(_spu_RXX + 0x1D6) = attr->regs[11];
    }
    if (setAll || (mask & 0x00001000)) {
        *(u16 *)(_spu_RXX + 0x1D8) = attr->regs[12];
    }
    if (setAll || (mask & 0x00002000)) {
        *(u16 *)(_spu_RXX + 0x1DA) = attr->regs[13];
    }
    if (setAll || (mask & 0x00004000)) {
        *(u16 *)(_spu_RXX + 0x1DC) = attr->regs[14];
    }
    if (setAll || (mask & 0x00008000)) {
        *(u16 *)(_spu_RXX + 0x1DE) = attr->regs[15];
    }
    if (setAll || (mask & 0x00010000)) {
        *(u16 *)(_spu_RXX + 0x1E0) = attr->regs[16];
    }
    if (setAll || (mask & 0x00020000)) {
        *(u16 *)(_spu_RXX + 0x1E2) = attr->regs[17];
    }
    if (setAll || (mask & 0x00040000)) {
        *(u16 *)(_spu_RXX + 0x1E4) = attr->regs[18];
    }
    if (setAll || (mask & 0x00080000)) {
        *(u16 *)(_spu_RXX + 0x1E6) = attr->regs[19];
    }
    if (setAll || (mask & 0x00100000)) {
        *(u16 *)(_spu_RXX + 0x1E8) = attr->regs[20];
    }
    if (setAll || (mask & 0x00200000)) {
        *(u16 *)(_spu_RXX + 0x1EA) = attr->regs[21];
    }
    if (setAll || (mask & 0x00400000)) {
        *(u16 *)(_spu_RXX + 0x1EC) = attr->regs[22];
    }
    if (setAll || (mask & 0x00800000)) {
        *(u16 *)(_spu_RXX + 0x1EE) = attr->regs[23];
    }
    if (setAll || (mask & 0x01000000)) {
        *(u16 *)(_spu_RXX + 0x1F0) = attr->regs[24];
    }
    if (setAll || (mask & 0x02000000)) {
        *(u16 *)(_spu_RXX + 0x1F2) = attr->regs[25];
    }
    if (setAll || (mask & 0x04000000)) {
        *(u16 *)(_spu_RXX + 0x1F4) = attr->regs[26];
    }
    if (setAll || (mask & 0x08000000)) {
        *(u16 *)(_spu_RXX + 0x1F6) = attr->regs[27];
    }
    if (setAll || (mask & 0x10000000)) {
        *(u16 *)(_spu_RXX + 0x1F8) = attr->regs[28];
    }
    if (setAll || (mask & 0x20000000)) {
        *(u16 *)(_spu_RXX + 0x1FA) = attr->regs[29];
    }
    if (setAll || (mask & 0x40000000)) {
        *(u16 *)(_spu_RXX + 0x1FC) = attr->regs[30];
    }
    if (setAll || (mask & 0x80000000)) {
        *(u16 *)(_spu_RXX + 0x1FE) = attr->regs[31];
    }
}
