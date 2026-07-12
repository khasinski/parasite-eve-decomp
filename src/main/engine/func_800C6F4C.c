typedef unsigned short u16;
typedef unsigned int u32;

extern u32 D_800E2370[];

void func_800C6F4C(char *data) {
    int i = 0;
    u32 *src = D_800E2370;
    u32 *dst = (u32 *)(data + *(u16 *)(data + 0x8));

    while (i < *(u16 *)(data + 0xA)) {
        i++;
        *dst++ = *src++;
    }
}
