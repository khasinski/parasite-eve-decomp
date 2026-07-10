/* MASPSX_FLAGS: --expand-div */

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

extern u32 *D_8009D2C8;

void SeqOp_SetTrack38Mask(void *track);

void SeqOp_NoteOnWithPitchSlide(void *track) {
    register u8 *base asm("$6");
    u8 *seq;
    int value;
    int step_count;
    int pitch_base;
    register int mask asm("$4");
    register int voice_index asm("$7");
    register int check asm("$2");

    base = (u8 *)track;
    asm volatile("" : "=r"(base) : "0"(base));

    seq = *(u8 **)base;
    *(u8 **)base = seq + 1;
    step_count = seq[0];
    *(u16 *)(base + 0x60) = step_count;
    if (step_count == 0) {
        step_count = 0x100;
        *(u16 *)(base + 0x60) = step_count;
    }

    seq = *(u8 **)base;
    pitch_base = *(u16 *)(base + 0x5E) & 0xFF00;
    *(u8 **)base = seq + 1;
    value = ((int)(seq[0] << 24) >> 16) - pitch_base;
    *(u16 *)(base + 0x5E) = pitch_base;
    *(u16 *)(base + 0xD6) = value / *(u16 *)(base + 0x60);

    if ((*(u32 *)(base + 0x38) & 0x800) == 0) {
        voice_index = 0;
        mask = 1;
        asm volatile("" : "=r"(mask) : "0"(mask));
        check = (int)D_8009D2C8;
        asm volatile(
            ".word 0x3C0800FF\n"
            ".word 0x8C430004\n"
            ".word 0x8C420030\n"
            ".word 0x3508FFFF\n"
            ".word 0x00621825\n"
            ".word 0x00641024\n"
            ".word 0x10400006\n"
            ".word 0x3C0200FF\n"
            ".word 0x00042040\n"
            ".word 0x00881024\n"
            ".word 0x1440FFFA\n"
            ".word 0x24E70001\n"
            ".word 0x3C0200FF\n"
            ".word 0x3442FFFF\n"
            ".word 0x00821024\n"
            : "=r"(check), "=r"(mask), "=r"(voice_index)
            : "0"(check), "1"(mask), "2"(voice_index)
            : "$3", "$8");

        if (check != 0) {
            D_8009D2C8[0xC] |= mask;
            *(u16 *)(base + 0x5C) = voice_index;
            *(u32 *)(base + 0x38) |= 0x800;
        }
    }

    SeqOp_SetTrack38Mask(base);
}
