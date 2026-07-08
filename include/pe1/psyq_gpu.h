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

/*
 * Many GPU helpers in src/main/main currently keep local callback-table
 * structs because register allocation is sensitive there. Centralize RECT
 * first; migrate callback declarations only after a function cluster is
 * build-checked.
 */

#endif
