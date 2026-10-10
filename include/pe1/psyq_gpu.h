#ifndef PE1_PSYQ_GPU_H
#define PE1_PSYQ_GPU_H

#include "pe1/psyq_types.h"

#define PE1_GPU_STATIC_ASSERT(expr, name) \
    typedef char pe1_gpu_static_assert_##name[(expr) ? 1 : -1]
#define PE1_GPU_OFFSETOF(type, member) ((unsigned int)&(((type *)0)->member))

typedef struct RECT {
    short x;
    short y;
    short w;
    short h;
} RECT;

/* The GPU command stream consumes each RECT as two packed words. */
typedef struct GpuRectWords {
    unsigned int position;
    unsigned int size;
} GpuRectWords;
PE1_GPU_STATIC_ASSERT(sizeof(GpuRectWords) == sizeof(RECT), rect_words_size);
PE1_GPU_STATIC_ASSERT(PE1_GPU_OFFSETOF(GpuRectWords, size) == 4, rect_words_size_offset);

typedef struct RECT32 {
    int x;
    int y;
    int w;
    int h;
} RECT32;

typedef struct GpuCmdPacket {
    union {
        int tag;
        struct {
            unsigned char pad0[3];
            unsigned char code;
        } head;
    } u0;
    int field4;
    int field8;
} GpuCmdPacket;

typedef struct DR_ENV {
    u_long tag;
    u_long code[15];
} DR_ENV;

typedef struct DRAWENV {
    RECT clip;
    short ofs[2];
    RECT tw;
    u_short tpage;
    u_char dtd;
    u_char dfe;
    u_char isbg;
    u_char r0;
    u_char g0;
    u_char b0;
    DR_ENV dr_env;
} DRAWENV;

typedef struct DISPENV {
    RECT disp;
    RECT screen;
    u_char isinter;
    u_char isrgb24;
    u_char pad0;
    u_char pad1;
} DISPENV;

PE1_GPU_STATIC_ASSERT(sizeof(DRAWENV) == 0x5C, drawenv_size);
PE1_GPU_STATIC_ASSERT(sizeof(DISPENV) == 0x14, dispenv_size);
#undef PE1_GPU_OFFSETOF
#undef PE1_GPU_STATIC_ASSERT

DRAWENV *GetDrawEnv(DRAWENV *env);
DRAWENV *PutDrawEnv(DRAWENV *env);
void DrawOTagEnv(void *next, DRAWENV *env);
DISPENV *PutDispEnv(DISPENV *env);
u_short GetClut(int x, int y);

/*
 * Many GPU helpers in src/main/main currently keep local callback-table
 * structs because register allocation is sensitive there. Centralize RECT
 * first; migrate callback declarations only after a function cluster is
 * build-checked.
 */

#endif
