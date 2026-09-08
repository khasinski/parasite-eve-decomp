#include "pe1/psyq_spu_internal.h"

void _spu_FsetPCR(s32 high_priority) {
    *_spu_sys_pcr &= 0xFFF8FFFF;

    if (high_priority) {
        *_spu_sys_pcr |= 0x30000;
    } else {
        *_spu_sys_pcr |= 0x50000;
    }
}

void _spu_FsetDelayW(void) {
    *_spu_delay = (*_spu_delay & 0xF0FFFFFF) | 0x20000000;
}

void _spu_FsetDelayR(void) {
    *_spu_delay = (*_spu_delay & 0xF0FFFFFF) | 0x22000000;
}


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
