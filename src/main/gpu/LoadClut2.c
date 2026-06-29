/* MASPSX_FLAGS: --stack-return-delay */
typedef unsigned short u_short;
typedef unsigned long u_long;

typedef struct {
    short x;
    short y;
    short w;
    short h;
} RECT;

int LoadImage(RECT *rect, u_long *p);
u_short GetClut(int x, int y);

u_short LoadClut2(u_long *p, int x, int y) {
    RECT rect;

    rect.x = x;
    rect.y = y;
    rect.w = 0x10;
    rect.h = 1;
    LoadImage(&rect, p);
    return GetClut(x, y);
}
