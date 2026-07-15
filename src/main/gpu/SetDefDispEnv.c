typedef short s16;
typedef unsigned char u8;

typedef struct {
    s16 x;
    s16 y;
    s16 w;
    s16 h;
} RECT;

typedef struct {
    RECT disp;
    RECT screen;
    u8 isinter;
    u8 isrgb24;
    u8 pad0;
    u8 pad1;
} DISPENV;

DISPENV *SetDefDispEnv(DISPENV *env, int x, int y, int w, int h)
{
    env->disp.x = x;
    env->disp.y = y;
    env->disp.w = w;
    env->disp.h = h;
    env->screen.x = 0;
    env->screen.y = 0;
    env->screen.w = 0;
    env->screen.h = 0;
    env->isrgb24 = 0;
    env->isinter = 0;
    env->pad1 = 0;
    env->pad0 = 0;
    return env;
}
