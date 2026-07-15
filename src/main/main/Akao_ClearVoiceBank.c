typedef signed char s8;
typedef unsigned char u8;
typedef short s16;
typedef unsigned short u16;
typedef int s32;

extern u8 D_800B0CD8[];
extern u8 D_80094488[];

void Akao_ClearVoiceBank(void) {
    u8 *state;
    u8 *entry;
    int i;
    int base_addr;

    state = D_800B0CD8;

    *(s32 *)(state + 0x000) = 3;
    *(s16 *)(state + 0x004) = 0xA;
    *(s16 *)(state + 0x006) = -1;
    state[0x008] = 2;
    state[0x00A] = 0xB;
    state[0x009] = -1;
    state[0x00B] = 0;
    state[0x00C] = -1;
    state[0x00D] = 1;
    state[0x00E] = 0;
    state[0x00F] = 0;
    state[0x010] = 0;
    state[0x011] = 0;
    state[0x012] = 0;
    state[0x013] = 0;

    for (i = 0; i < 0x31; i++) {
        *(s32 *)(state + 0x14 + i * 4) = 0;
    }

    state[0x0D9] = 8;
    state[0x0D8] = 0;
    state[0x0DB] = -1;
    state[0x0DA] = -1;

    for (i = 0; i < 2; i++) {
        state[0x0DC + i * 2] = -1;
        state[0x0DD + i * 2] = -1;
    }

    state[0x0FF] = 0x7F;
    state[0x0FE] = 0x7F;
    state[0x0E0] = 0x27;
    state[0x0E1] = 0x0D;
    state[0x0E3] = 1;
    state[0x0E6] = 0x98;
    state[0x0E2] = 0;
    *(s16 *)(state + 0x0E4) = -1;
    state[0x0E7] = -1;
    *(s16 *)(state + 0x0E8) = -1;
    state[0x0EB] = 0;
    state[0x0EA] = 0;

    entry = D_80094488;
    for (i = 0; i < 0x20; i += 8, entry += 8) {
        *(u16 *)(entry + 6) = 0;
        *(u16 *)(D_80094488 + 4 + i) = 0;
    }

    state[0x0F6] = 0x30;
    state[0x0F7] = 0x7F;
    *(s16 *)(state + 0x0F8) = 0x100;
    *(s16 *)(state + 0x0FA) = 0x800;
    state[0x107] = 3;
    state[0x10B] = 0x60;
    *(s16 *)(state + 0x110) = 0x140;
    *(s16 *)(state + 0x112) = 0xE0;
    state[0x117] = 1;

    state[0x0ED] = 0x20;
    state[0x108] = 0x20;
    state[0x109] = 0x20;
    state[0x10A] = 0x20;
    base_addr = *(s32 *)(state + 0x150);
    state[0x0EC] = 0;
    state[0x0EE] = 0;
    state[0x0EF] = 0;
    state[0x0F0] = 0;
    state[0x0F1] = 0;
    state[0x0F4] = 0;
    state[0x0F2] = 0;
    state[0x0F4] = 0;
    *(s16 *)(state + 0x10C) = 0;
    *(s16 *)(state + 0x10E) = 0;
    *(s32 *)(state + 0x118) = 0xE1000440;
    *(s32 *)(state + 0x11C) = 0;
    *(s32 *)(state + 0x120) = 0;
    *(s32 *)(state + 0x124) = 0;
    state[0x10B] |= 2;
    *(s32 *)(state + 0x128) = base_addr;
    *(s32 *)(state + 0x12C) = base_addr + 0x1400;
    *(s32 *)(state + 0x130) = base_addr + 0x2800;

    for (i = 2, entry = state + 8; i >= 0; i--, entry -= 4) {
        *(s32 *)(entry + 0x134) = 0;
    }

    for (i = 1, entry = state + 4; i >= 0; i--, entry -= 4) {
        *(s32 *)(entry + 0x140) = 0;
    }

    *(s32 *)(state + 0x148) = 0;
}
