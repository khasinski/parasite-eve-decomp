#ifndef PE1_MATH64_H
#define PE1_MATH64_H

/* Little-endian 64-bit intermediate used by the software math helpers. */
typedef struct MathU64 {
    unsigned int lo;
    unsigned int hi;
} MathU64;

#endif
