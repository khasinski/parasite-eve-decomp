#ifndef PE1_PSYQ_LIBC_H
#define PE1_PSYQ_LIBC_H

#include "common.h"

/* Retail formatter takes a destination followed by a signed-byte format. */
s32 sprintf(char *destination, s8 *format, ...);
void *memset(void *dst, int value, unsigned int size);
int rand(void);
void srand(unsigned int seed);

/* Comparators receive addresses of two elements in the sorted array. */
typedef int (*PsyqSortCompare)(const void *left, const void *right);
void qsort(void *base, unsigned int count, unsigned int size,
           PsyqSortCompare compare);

#endif /* PE1_PSYQ_LIBC_H */
