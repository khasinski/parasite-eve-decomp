#include "common.h"
#include "pe1/psyq_tim.h"
extern signed char g_DrawEnabled;
extern signed char D_800B0DBB;
extern u16 g_SeqElapsed;


int Gpu_CheckDrawStatus(void) {
    int enabled = g_DrawEnabled;
    register int value asm("$2");
    int result;
    register int flag asm("$3");

    /* Matching debt: retail reserves an otherwise unused eight-byte frame. */
    volatile int matchingStackReserve[2];
    /* Preserve retail's second comparison of the enabled snapshot. */
    flag = enabled;
    if (enabled != 0) {
        asm volatile("" : "=r"(flag) : "0"(flag));
        value = -1;
        if (flag != 0) {
            value = g_SeqElapsed;
            value = (unsigned int)value << 16;
        } else {
            asm("" : "=r"(value) : "0"(value));
            value = (unsigned int)value << 16;
        }
        if (value > 0) {
            flag = D_800B0DBB;
            if (flag != 0) {
                result = 2;
                goto done;
            }
            result = 1;
            goto done;
        }
    }

    result = 0;

done:
    return result;
}

int Gpu_GetTimTableEntry(int base, int index) {
    int offset = (short)index * 4;
    int ptr = (unsigned int)offset + (unsigned int)base;
    return (unsigned int)base + (unsigned int)*(int *)ptr;
}

void Gpu_LoadTimTable(int base, int count) {
    int i;

    for (i = 0; i < count; i++) {
        int ptr = (unsigned int)((short)i * 4) + (unsigned int)base;
        int offset = *(int *)ptr;
        Gpu_LoadTimImage((TimFile *)((unsigned int)base + (unsigned int)offset));
    }
}
