/* MASPSX_FLAGS: --expand-div */
extern int D_8019CA90;
extern int D_8019CB48;
extern int D_8019CB4C;
extern int D_8019CB50;
extern int D_8019CB54;
extern int D_8019CB58;
extern int D_8019CBA8;
extern int D_8019CBAC;
extern int D_8019CBB0;
extern int D_8019CBB4;
extern int D_8019CBB8;
extern int D_8019CBC4;
extern int D_8019CBC8;
extern int D_8019CBD0;
extern int D_8019CBD4;
extern int D_8019CBD8;
extern int D_8019CBF0;
extern int D_8019CBF4;
extern int D_8019CBF8;
extern int D_8019CC00;
extern int D_8019CC04;
extern int D_8019CC08;
extern int D_8019CC0C;
extern int D_8019CC10;
int FxCommon_CheckFourBoundsWithMargin(int *point, int margin) {
    int first, second, third, fourth, distance;
    int limit;
    int fourthVisible = 0, thirdVisible = 0, secondVisible = 0, firstVisible = 0;
    int threshold = margin;
    first = D_8019CBB0 * point[0] + D_8019CBB4 * point[1]
          + D_8019CBB8 * point[2] + D_8019CB48;
    second = D_8019CBD0 * point[0] + D_8019CBD4 * point[1]
          + D_8019CBD8 * point[2] + D_8019CB4C;
    third = D_8019CBF0 * point[0] + D_8019CBF4 * point[1]
          + D_8019CBF8 * point[2] + D_8019CBA8;
    fourth = D_8019CB50 * point[0] + D_8019CB54 * point[1]
          + D_8019CB58 * point[2] + D_8019CA90;
    if (D_8019CC04 > 0 && first >= 0)
        firstVisible = 1;
    if (D_8019CC04 < 0 && first <= 0)
        firstVisible = 1;
    if (D_8019CC0C > 0 && second >= 0)
        secondVisible = 1;
    if (D_8019CC0C < 0 && second <= 0)
        secondVisible = 1;
    if (D_8019CC10 > 0 && third >= 0)
        thirdVisible = 1;
    if (D_8019CC10 < 0 && third <= 0)
        thirdVisible = 1;
    if (D_8019CBC4 > 0 && fourth >= 0)
        fourthVisible = 1;
    if (D_8019CBC4 < 0 && fourth <= 0)
        fourthVisible = 1;
    limit = threshold * 8;
    threshold = limit + 30;
    if (!firstVisible) {
        if (first >= 0)
            distance = first / D_8019CBC8;
        else
            distance = -first / D_8019CBC8;
        limit = (short)threshold;
        if (distance < limit)
            firstVisible = 1;
    }
    if (!secondVisible) {
        if (second >= 0)
            distance = second / D_8019CC00;
        else
            distance = -second / D_8019CC00;
        limit = (short)threshold;
        if (distance < limit)
            secondVisible = 1;
    }
    if (!thirdVisible) {
        if (third >= 0)
            distance = third / D_8019CC08;
        else
            distance = -third / D_8019CC08;
        limit = (short)threshold;
        if (distance < limit)
            thirdVisible = 1;
    }
    if (!fourthVisible) {
        if (fourth >= 0)
            distance = fourth / D_8019CBAC;
        else
            distance = -fourth / D_8019CBAC;
        limit = (short)threshold;
        if (distance < limit)
            fourthVisible = 1;
    }
    return firstVisible & secondVisible & thirdVisible & fourthVisible;
}
