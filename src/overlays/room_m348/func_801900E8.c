typedef unsigned char u8;
typedef unsigned short u16;

#define RW8(o, off) (*(u8 *)((char *)(o) + (off)))
#define RWU16(o, off) (*(u16 *)((char *)(o) + (off)))

void func_801900E8(void *unused, u8 *signal, char *state) {
    u16 h10;
    u16 h12;
    u16 h16;
    u16 h14;

    h10 = RWU16(state, 0x10);
    h12 = RWU16(state, 0x12);
    h16 = RWU16(state, 0x16);
    RWU16(state, 0x10) = h10 + 0x64;
    RWU16(state, 0x12) = h12 + h16;
    RWU16(state, 0x16) = RWU16(state, 0x16) + 0x32;
    h14 = RWU16(state, 0x14) - 0x10;
    RWU16(state, 0x14) = h14;
    if ((short)h14 < 0) {
        signal[1] = 2;
        RWU16(state, 0x14) = 0;
    }
}
