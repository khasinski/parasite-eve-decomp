/* GCC_VERSION: 2.8.1 */
#include "common.h"
void Mem_SwapBuffers(void *, void *, unsigned int);

void qsort(void *base, unsigned int count, unsigned int size,
           int (*compare)(const void *, const void *))
{
    register char *pivot asm("$20") = base;
    char *current = pivot + size;
    register unsigned int smaller asm("$22") = 0;
    char *last;
    unsigned int i;
    unsigned int half;

    if (count < 2) return;
    if (count == 2) {
        if (compare(pivot, current) > 0) {
            Mem_SwapBuffers(pivot, current, size);
        }
        return;
    }
    half = count >> 1;
    /* Keep the pivot reload after the two-element path, with the shift before it. */
    asm volatile("" : "+r"(pivot) : "r"(half));
    Mem_SwapBuffers(pivot, pivot + half * size, size);
    last = pivot;
    for (i = 1; i < count; i++, current += size) {
        if (compare(current, pivot) < 0) {
            last += size;
            smaller++;
            if (current != last) Mem_SwapBuffers(current, last, size);
        }
    }
    if (last != pivot) Mem_SwapBuffers(pivot, last, size);
    qsort(pivot, smaller, size, compare);
    qsort(last + size, count - smaller - 1, size, compare);
}
