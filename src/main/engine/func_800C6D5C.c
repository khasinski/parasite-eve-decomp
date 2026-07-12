typedef unsigned char u8;
typedef unsigned short u16;
typedef signed short s16;

extern s16 D_800F3420;

void func_800C6D5C(u8 *data, u8 xOffset, u8 yOffset) {
    u8 *ptr = data + 0x10;
    int i;

    if (*(u16 *)(data + 0xC) != xOffset || *(u16 *)(data + 0xE) != yOffset) {
        *(u16 *)(data + 0xC) = xOffset;
        *(u16 *)(data + 0xE) = yOffset;

        for (i = 0; i < *(u16 *)(data + 0) + *(u16 *)(data + 2); i++, ptr += 0xC) {
            ptr[6] += xOffset;
            ptr[8] += xOffset;
            ptr[0xA] += xOffset;
            ptr[7] += yOffset;
            ptr[9] += yOffset;
            ptr[0xB] += yOffset;
        }

        for (i = 0; i < *(u16 *)(data + 4) + *(u16 *)(data + 6); i++, ptr += 0x10) {
            ptr[8] += xOffset;
            ptr[0xA] += xOffset;
            ptr[0xC] += xOffset;
            ptr[0xE] += xOffset;
            ptr[9] += yOffset;
            ptr[0xB] += yOffset;
            ptr[0xD] += yOffset;
            ptr[0xF] += yOffset;
        }
    }

    D_800F3420 = 0;
}
