#ifndef PE1_RENDER_PRIM_H
#define PE1_RENDER_PRIM_H

/* A render primitive list and its 0x10-byte color records (Render_SetPrimColour).
 * The stride 0x10 is load-bearing (code indexes entries with i<<4). */
typedef struct PrimEntry {            /* 0x10 */
    unsigned char pad_00[0x04];
    unsigned char r;                  /* 0x04 */
    unsigned char g;                  /* 0x05 */
    unsigned char b;                  /* 0x06 */
    unsigned char pad_07[0x09];
} PrimEntry;

typedef struct PrimObj {
    unsigned char pad_00[0x26];
    unsigned short count;             /* 0x26 */
    unsigned char pad_28[0x08];
    PrimEntry *entries;               /* 0x30 */
} PrimObj;

#endif /* PE1_RENDER_PRIM_H */
