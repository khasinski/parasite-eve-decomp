/* MASPSX_FLAGS: --expand-div */

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

extern char *g_AkaoCurTrack;

void SeqOp_SetVolumeSlideTarget(void *ptr) {
    register void *stream asm("$5");
    u8 *pc;
    register int value asm("$2");

    stream = ptr;

    {
        register u8 *pc0 asm("$2");
        register char *track asm("$3");

        pc0 = *(u8 **)stream;
        *(u8 **)stream = pc0 + 1;
        track = g_AkaoCurTrack;
        value = pc0[0];
        *(u16 *)(track + 0x58) = value;
        if (value == 0) {
            value = 0x100;
            *(u16 *)(track + 0x58) = value;
        }
    }

    {
        register int high asm("$3");
        char *track;
        int current;

        pc = *(u8 **)stream;
        *(u8 **)stream = pc + 1;
        value = pc[0];
        *(u8 **)stream = pc + 2;
        high = pc[1];

        track = g_AkaoCurTrack;
        value <<= 16;
        high <<= 24;
        value |= high;

        current = *(u32 *)(track + 0x40);
        current &= 0xFFFF0000;
        *(u32 *)(track + 0x40) = current;
        *(u32 *)(track + 0x44) = (value - current) / *(u16 *)(track + 0x58);
    }
}
