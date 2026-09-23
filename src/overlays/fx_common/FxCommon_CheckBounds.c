extern int D_8019CBB0, D_8019CBB4, D_8019CBB8;
extern int D_8019CBD0, D_8019CBD4, D_8019CBD8;
extern int D_8019CB48, D_8019CB4C;
extern int D_8019CC04, D_8019CC0C;

int func_8018FFF4(int *point)
{
    int first, second;
    int firstVisible = 0;
    register int secondVisible asm("$5") = 0;

    first = D_8019CBB0 * point[0] + D_8019CBB4 * point[1]
          + D_8019CBB8 * point[2] + D_8019CB48;
    second = D_8019CBD0 * point[0] + D_8019CBD4 * point[1]
           + D_8019CBD8 * point[2] + D_8019CB4C;

    if (first > 0 && D_8019CC04 > 0)
        firstVisible = 1;
    if (first < 0 && D_8019CC04 < 0)
        firstVisible = 1;
    if (second > 0 && D_8019CC0C > 0)
        secondVisible = 1;
    if (second < 0 && D_8019CC0C < 0)
        secondVisible = 1;

    return firstVisible & secondVisible;
}

int func_80190124(short *point)
{
    int first, second;
    int firstVisible = 0;
    register int secondVisible asm("$5") = 0;

    first = D_8019CBB0 * point[0] + D_8019CBB4 * point[1]
          + D_8019CBB8 * point[2] + D_8019CB48;
    second = D_8019CBD0 * point[0] + D_8019CBD4 * point[1]
           + D_8019CBD8 * point[2] + D_8019CB4C;

    if (first > 0 && D_8019CC04 > 0)
        firstVisible = 1;
    if (first < 0 && D_8019CC04 < 0)
        firstVisible = 1;
    if (second > 0 && D_8019CC0C > 0)
        secondVisible = 1;
    if (second < 0 && D_8019CC0C < 0)
        secondVisible = 1;

    return firstVisible & secondVisible;
}
