/* MASPSX_FLAGS: --stack-return-delay */
typedef short s16;

typedef struct {
    s16 x;
    s16 y;
    s16 w;
    s16 h;
} RECT;

extern void LoadImage(RECT *rect, void *data);
extern int GetClut(int x, int y);

int func_8007485C(void *data, int x, int y)
{
    RECT rect;

    rect.x = x;
    rect.y = y;
    rect.w = 0x100;
    rect.h = 1;
    LoadImage(&rect, data);
    return GetClut(x, y) & 0xFFFF;
}
