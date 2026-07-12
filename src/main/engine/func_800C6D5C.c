typedef unsigned char u8;
typedef unsigned short u16;
typedef signed short s16;

extern s16 D_800F3420;

void func_800C6D5C(u8 *data, u8 xOffset, u8 yOffset) {
    register u8 *data_t0 asm("$8");
    register u8 *ptr_t2 asm("$10");
    register int value_v0 asm("$2");
    register int value_v1 asm("$3");
    register int x_a0 asm("$4");
    register int i_t1 asm("$9");
    register int count_a0 asm("$4");
    int total;

    asm volatile("" : "=r"(data_t0) : "0"(data));
    value_v0 = *(u16 *)(data_t0 + 0xC);
    asm volatile("" : "=r"(value_v0) : "0"(value_v0));
    x_a0 = xOffset & 0xFF;
    ptr_t2 = data_t0 + 0x10;

    if (value_v0 != x_a0 || *(u16 *)(data_t0 + 0xE) != (yOffset & 0xFF)) {
        i_t1 = 0;
        *(u16 *)(data_t0 + 0xC) = x_a0;
        value_v0 = *(u16 *)(data_t0 + 0);
        count_a0 = *(u16 *)(data_t0 + 2);
        value_v1 = yOffset & 0xFF;
        value_v0 += count_a0;
        *(u16 *)(data_t0 + 0xE) = value_v1;
        if (value_v0 != 0) {
            register u8 *ptr_a3 asm("$7");

            ptr_a3 = data_t0 + 0x1B;
            do {
                i_t1++;
                ptr_t2 += 0xC;
                ptr_a3[-5] += xOffset;
                ptr_a3[-3] += xOffset;
                ptr_a3[-1] += xOffset;
                ptr_a3[-4] += yOffset;
                ptr_a3[-2] += yOffset;
                ptr_a3[0] += yOffset;
                total = *(u16 *)(data_t0 + 0);
                count_a0 = *(u16 *)(data_t0 + 2);
                total += count_a0;
                ptr_a3 += 0xC;
            } while ((u16)i_t1 < total);
        }

        i_t1 = 0;
        asm volatile("" : "=r"(i_t1) : "0"(i_t1));
        value_v0 = *(u16 *)(data_t0 + 4);
        value_v1 = *(u16 *)(data_t0 + 6);
        value_v0 += value_v1;
        count_a0 = (int)ptr_t2;
        if (value_v0 != 0) {
            register u8 *ptr_a3 asm("$7");

            ptr_a3 = (u8 *)count_a0 + 0xF;
            do {
                i_t1++;
                ptr_a3[-7] += xOffset;
                ptr_a3[-5] += xOffset;
                ptr_a3[-3] += xOffset;
                ptr_a3[-1] += xOffset;
                ptr_a3[-6] += yOffset;
                ptr_a3[-4] += yOffset;
                ptr_a3[-2] += yOffset;
                ptr_a3[0] += yOffset;
                total = *(u16 *)(data_t0 + 4);
                count_a0 = *(u16 *)(data_t0 + 6);
                total += count_a0;
                ptr_a3 += 0x10;
            } while ((u16)i_t1 < total);
        }
    }

    D_800F3420 = 0;
}
