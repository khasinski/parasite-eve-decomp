#ifndef PE1_PSYQ_GPU_H
#define PE1_PSYQ_GPU_H

typedef signed char s_char;
typedef unsigned char u_char;
typedef unsigned short u_short;
typedef unsigned long u_long;

typedef struct RECT {
    short x;
    short y;
    short w;
    short h;
} RECT;

typedef struct RECT32 {
    int x;
    int y;
    int w;
    int h;
} RECT32;

/*
 * Many GPU helpers in src/main/main currently keep local callback-table
 * structs because register allocation is sensitive there. Centralize RECT
 * first; migrate callback declarations only after a function cluster is
 * build-checked.
 */

#endif
