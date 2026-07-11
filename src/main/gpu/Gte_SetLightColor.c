typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

void Gte_SetLightColor(u8 *state, int index, int r, int g, int b) {
    u32 *matrix;
    u8 *byte_slot;

    if ((unsigned int)index < 3) {
        *(u16 *)(state + 0x20 + index * 2) = r;
        *(u16 *)(state + 0x26 + index * 2) = g;
        *(u16 *)(state + 0x2C + index * 2) = b;
    }

    byte_slot = state + 0x40 + index * 4;
    byte_slot[0] = r;
    byte_slot[1] = g;
    byte_slot[2] = b;

    matrix = (u32 *)(state + 0x20);
    asm volatile("ctc2 %0,$16" : : "r"(matrix[0]));
    asm volatile("ctc2 %0,$17" : : "r"(matrix[1]));
    asm volatile("ctc2 %0,$18" : : "r"(matrix[2]));
    asm volatile("ctc2 %0,$19" : : "r"(matrix[3]));
    asm volatile("ctc2 %0,$20" : : "r"(matrix[4]));
}
