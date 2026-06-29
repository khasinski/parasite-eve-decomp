
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

extern void *g_AkaoSoundEntryTable[];

void SeqOp_LoadSoundEntry(void *ptr) {
    u8 *pc;
    int duration;
    int selector;

    *(u32 *)((char *)ptr + 0x38) |= 4;

    pc = *(u8 **)ptr;
    *(u8 **)ptr = pc + 1;
    duration = pc[0];
    *(u16 *)((char *)ptr + 0xAE) = duration;
    if (duration == 0) {
        *(u16 *)((char *)ptr + 0xAE) = 0x100;
    }

    pc = *(u8 **)ptr;
    *(u8 **)ptr = pc + 1;
    selector = pc[0];
    *(u16 *)((char *)ptr + 0xB2) = selector;
    {
        void *entry = g_AkaoSoundEntryTable[selector];
        *(u16 *)((char *)ptr + 0xB0) = 1;
        *(void **)((char *)ptr + 0x24) = entry;
    }
}
