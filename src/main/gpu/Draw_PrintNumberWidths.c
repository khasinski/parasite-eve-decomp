/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
extern int D_8009D124;
extern int D_8009D128;

void Draw_AllocTexturedRect(int value, int width);

void Draw_PrintNumberWidth6(int value) {
    int x;
    int y;

    Draw_AllocTexturedRect(value, 6);
    x = D_8009D124;
    y = D_8009D128;
    D_8009D124 = x + 6;
    D_8009D128 = y;
}

void Draw_PrintNumberWidth5(int value) {
    int x;
    int y;

    Draw_AllocTexturedRect(value, 5);
    x = D_8009D124;
    y = D_8009D128;
    D_8009D124 = x + 5;
    D_8009D128 = y;
}

void Draw_PrintNumberWidth4(int value) {
    int x;
    int y;

    Draw_AllocTexturedRect(value, 4);
    x = D_8009D124;
    y = D_8009D128;
    D_8009D124 = x + 4;
    D_8009D128 = y;
}

void func_800605C4(int value) {
    int x;
    int y;

    Draw_AllocTexturedRect(value, 3);
    x = D_8009D124;
    y = D_8009D128;
    D_8009D124 = x + 3;
    D_8009D128 = y;
}

void Draw_PrintNumberWidth2(int value) {
    int x;
    int y;

    Draw_AllocTexturedRect(value, 2);
    x = D_8009D124;
    y = D_8009D128;
    D_8009D124 = x + 2;
    D_8009D128 = y;
}
