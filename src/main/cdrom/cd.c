
#include "include_asm.h"
#include "pe1/psyq_cd.h"


void parcpy(unsigned char *dst, unsigned char *src) {
    int i;

    if (src) {
        if (dst) {
            for (i = 0; i < 4; i++) {
                *dst++ = *src++;
            }
        }
    } else if (dst) {
        *dst = 0;
    }
}

void rescpy(unsigned char *dst, unsigned char *src) {
    int i;

    if (src) {
        if (dst) {
            for (i = 0; i < 8; i++) {
                *dst++ = *src++;
            }
        }
    } else if (dst) {
        *dst = 0;
    }
}
