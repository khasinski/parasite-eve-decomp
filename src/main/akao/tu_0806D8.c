typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

extern void *g_AkaoSoundEntryTable[];

void SeqOp_StopPitchLFO(void *ptr) {
    *(short *)((char *)ptr + 0xE8) = 0;
    *(int *)((char *)ptr + 0x38) &= ~1;
    *(int *)((char *)ptr + 0xF4) |= 0x10;
}

void SeqOp_SetVolumeLFO(void *ptr) {
    u8 *pc;
    int value;
    int selector;
    int tmp;
    void *entry;

    *(u32 *)((char *)ptr + 0x38) |= 2;

    if (*(u16 *)((char *)ptr + 0x54) != 0) {
        *(u16 *)((char *)ptr + 0x9C) = 0;
        pc = *(u8 **)ptr;
        *(u8 **)ptr = pc + 1;
        value = pc[0];
        if (value != 0) {
            *(u16 *)((char *)ptr + 0xA6) = value << 8;
        }
    } else {
        *(u16 *)((char *)ptr + 0x9C) = *(*(u8 **)ptr)++;
    }

    /* The chained assignment and the *(*pp)++ consume idiom are load-bearing:
     * they keep the stream pointer temp in $v0 as retail allocates it. */
    value = (*(u16 *)((char *)ptr + 0xA0) = *(*(u8 **)ptr)++);
    if (value == 0) {
        *(u16 *)((char *)ptr + 0xA0) = 0x100;
    }

    pc = *(u8 **)ptr;
    *(u8 **)ptr = pc + 1;
    selector = pc[0];
    tmp = *(u16 *)((char *)ptr + 0x9C);
    *(u16 *)((char *)ptr + 0xA4) = selector;
    entry = g_AkaoSoundEntryTable[selector];
    *(u16 *)((char *)ptr + 0x9E) = tmp;
    *(u16 *)((char *)ptr + 0xA2) = 1;
    *(void **)((char *)ptr + 0x20) = entry;
}

void SeqOp_UpdateVolumeLFOTarget(void *ptr) {
    unsigned char *cursor = *(unsigned char **)ptr;

    *(unsigned char **)ptr = cursor + 1;
    *(short *)((char *)ptr + 0xA6) = *cursor << 8;
}
