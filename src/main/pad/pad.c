typedef short s16;

typedef struct PadWork {
    char pad[0xA];
    s16 unk_a;
} PadWork;

extern PadWork *D_8009B4B0;

int func_8007E0F8(void) {
    volatile int delay[4];
    PadWork *work;

    work = D_8009B4B0;
    work->unk_a = 0;

    delay[0] = 10;
    if (--delay[0] != -1) {
        do {
        } while (--delay[0] != -1);
    }

    return 0;
}
