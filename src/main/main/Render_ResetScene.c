typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;

extern u8 D_800BCDC8[];
extern u8 D_800BCE80[];
extern u8 D_800BCE94[];
extern u8 D_800BCE91;
extern u8 D_800BCEA5;
extern u16 D_800BCE9E;
extern u16 D_800BCE8A;
extern u16 D_800BCE9C;
extern u16 D_800BCE88;
extern u16 D_800BCEA2;
extern u16 D_800BCE8E;
extern u8 D_800BCE3C;
extern u8 D_800BCDE0;
extern u8 D_800BCE3A;
extern u8 D_800BCDDE;
extern u8 D_800BCE3B;
extern u8 D_800BCDDF;
extern u16 D_800BCE38;
extern u16 D_800BCDDC;
extern u8 D_800BCE3D;
extern u8 D_800BCDE1;
extern u8 D_800BCE3E;
extern u8 D_800BCDE2;
extern u8 D_800BCE3F;
extern u8 D_800BCDE3;
extern int D_8009CDDC;

void Render_InitEntityPool(int arg0);
void SetGraphDebug(int level);
void SetDispMask(int mask);
void ClearImage(s16 *rect, int r, int g, int b);
void SetDefDispEnv(void *env, int x, int y, int w, int h);
void SetDefDrawEnv(void *env, int x, int y, int w, int h);
void Render_StepEntityPool(void *arg0);

void Render_ResetScene(int width, int height) {
    s16 rect[4];
    u8 *disp;
    u8 *draw;

    Render_InitEntityPool(0);
    SetGraphDebug(0);
    SetDispMask(0);

    rect[0] = 0;
    rect[1] = 0;
    rect[2] = 0x3FF;
    rect[3] = 0x200;
    ClearImage(rect, 0, 0, 1);

    disp = D_800BCE80;
    D_800BCE91 = 1;
    D_800BCEA5 = 1;
    SetDefDispEnv(disp, 0, 0, height, width);
    SetDefDispEnv(D_800BCE94, 0, 0, height, width);

    draw = D_800BCDC8;
    D_800BCE9E = 8;
    D_800BCE8A = 8;
    D_800BCE9C = 0;
    D_800BCE88 = 0;
    D_800BCEA2 = height;
    D_800BCE8E = height;
    SetDefDrawEnv(draw, 0, 0, width, height);
    SetDefDrawEnv(draw + 0x5C, 0, height, width, height);

    D_800BCE3C = 1;
    D_800BCDE0 = 1;
    D_800BCE3A = 1;
    D_800BCDDE = 1;
    D_800BCE3B = 0;
    D_800BCDDF = 0;
    D_800BCE38 = 0;
    D_800BCDDC = 0;
    D_800BCE3D = 0;
    D_800BCDE1 = 0;
    D_800BCE3E = 0;
    D_800BCDE2 = 0;
    D_800BCE3F = 0;
    D_800BCDE3 = 0;
    D_8009CDDC = 0;

    Render_StepEntityPool(disp);
}
