#include "pe1/psyq_cd.h"

void mem2mem(void *destination, void *source, unsigned int count, int reserved) {
    int *dst = destination;
    int *src = source;
    unsigned int i;
    int value;

    i = 0;
    if (count != 0) {
        do {
            value = *src;
            src++;
            i++;
            *dst = value;
            dst++;
        } while (i < count);
    }
}
