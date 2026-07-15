typedef signed short s16;
typedef unsigned char u8;
typedef unsigned int u32;

void func_800C4E50(char *data);

void func_8018F550(void *arg0, void *arg1, char *state) {
    *(s16 *)(state + 0x120) = 0x5DC;
    *(s16 *)(state + 0x122) = 0xFF;
    *(s16 *)(state + 0x114) = 0x10;
    *(u8 *)(state + 0x10C) = 0x40;
    *(u8 *)(state + 0x10D) = 0x20;
    *(u8 *)(state + 0x10E) = 0x10;
    *(u8 *)(state + 0x110) = 0x80;
    *(u8 *)(state + 0x111) = 0x80;
    *(u8 *)(state + 0x112) = 0x80;
    *(s16 *)(state + 0x116) = 0xF0;
    *(s16 *)(state + 0x118) = 0xA;
    *(s16 *)(state + 0x124) = 0;
    *(s16 *)(state + 0x11A) = 0;
    *(u32 *)(state + 0x108) = (u32)(state + 0x8);
    func_800C4E50(state + 0x108);
}
