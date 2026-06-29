typedef signed short s16;
typedef unsigned char u8;
typedef unsigned short u16;

void SeqOp_JumpIfLoopCountPop(void *ptr) {
    void *track;
    u8 *cursor;
    register int value asm("$3");
    u8 *target;
    int offset;

    track = ptr;
    asm volatile("" : "=r"(track) : "0"(track));
    cursor = *(u8 **)track;
    *(u8 **)track = cursor + 1;
    value = cursor[0];
    if (value == 0) {
        value = 0x100;
    }

    if (*(u16 *)((char *)track + 0x62 + (*(u16 *)((char *)track + 0xCE) * 2)) + 1 != value) {
        u8 *skip;

        skip = cursor + 3;
        *(u8 **)track = skip;
        return;
    }

    *(u8 * volatile *)track = cursor + 2;
    value = cursor[1];
    target = cursor + 3;
    *(u8 **)track = target;
    offset = (s16)(value | (cursor[2] << 8));
    *(u8 **)track = target + offset;
    *(u16 *)((char *)track + 0xCE) = (*(u16 *)((char *)track + 0xCE) - 1) & 3;
}
