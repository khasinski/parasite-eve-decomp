typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

extern int D_8009CDDC;
extern u8 D_8009E46C[];
extern u8 D_8009E46D[];
extern u8 D_8009E46E[];
extern u32 *D_800B0E38[];

#define U8_AT(ptr, off) (*(u8 *)((u8 *)(ptr) + (off)))
#define S16_AT(ptr, off) (*(short *)((u8 *)(ptr) + (off)))
#define U16_AT(ptr, off) (*(u16 *)((u8 *)(ptr) + (off)))

void AddPrim(void *ot, void *prim);

int Battle_DrawDecimalNumber(void *base, int x, int y, int value, int use_status_colour) {
    u8 digits[8];
    int digit_count;
    int quotient;
    int current;
    int slot_offset;

    digit_count = 0;
    value = (short)value;
    quotient = value / 10;
    digits[0] = value - (quotient * 10);
    while ((short)quotient != 0) {
        int next;

        digit_count++;
        next = (short)quotient / 10;
        digits[(signed char)digit_count] = quotient - (next * 10);
        quotient = next;
    }

    x -= ((signed char)digit_count * 6);
    if ((signed char)digit_count >= 0) {
        current = digit_count;
        use_status_colour = (short)use_status_colour;
        do {
            u8 *prim = (u8 *)base + (((signed char)current * 7) * 4);
            int digit = digits[(signed char)current];

            U8_AT(prim, 0x15) = 0xF2;
            S16_AT(prim, 0x10) = x;
            S16_AT(prim, 0x12) = y;
            U8_AT(prim, 0x14) = digit << 3;

            if (use_status_colour == 1) {
                slot_offset = ((D_8009CDDC * 7) * 4);
                U8_AT(prim, 0xC) = D_8009E46C[slot_offset] + 0x28;
                U8_AT(prim, 0xD) = D_8009E46D[slot_offset];
                U8_AT(prim, 0xE) = D_8009E46E[slot_offset];
            }

            AddPrim(D_800B0E38[D_8009CDDC] + 4, prim);
            x += 6;
            current--;
        } while (((signed char)current) >= 0);
    }

    return (signed char)digit_count;
}
