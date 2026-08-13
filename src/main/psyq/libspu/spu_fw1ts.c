
typedef struct SpuWaitWork {
    volatile int i;
    volatile int value;
} SpuWaitWork;

void _spu_Fw1ts(void) {
    register SpuWaitWork *work asm("$29");
    int current;

    work--;
    work->value = 0xD;
    *(int *)&work->i = 0;
    while (work->i < 0x3C) {
        current = work->value;
        work->value = (((current << 1) + current) << 2) + current;
        work->i = work->i + 1;
    }
    work++;
}
