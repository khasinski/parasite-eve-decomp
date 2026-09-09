/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */

typedef struct SpuWaitWork {
    volatile int i;
    volatile int value;
} SpuWaitWork;

void _spu_Fw1ts(void) {
    SpuWaitWork work;
    int current;

    work.value = 0xD;
    *(int *)&work.i = 0;
    while (work.i < 0x3C) {
        current = work.value;
        work.value = (((current << 1) + current) << 2) + current;
        work.i = work.i + 1;
    }
}
