/* MASPSX_FLAGS: --expand-div */

extern short D_8009A6EC[];

long Gte_Atan2(long dx, long dy) {
    long ret;
    int flag0;
    int flag1;
    int lookup;

    flag0 = 0;
    flag1 = 0;
    if (dy < 0) {
        flag0 = 1;
        dy = -dy;
    }
    if (dx < 0) {
        flag1 = 1;
        dx = -dx;
    }
    if ((dy != 0) || (dx != 0)) {
        int cond;
        if (dy != 0) {
            cond = dx < dy;
        } else {
            cond = dx < dy;
        }
        if (cond) {
            if (dx & 0x7FE00000) {
                lookup = dx / (dy >> 10);
            } else {
                lookup = (dx << 10) / dy;
            }
            ret = D_8009A6EC[lookup];
        } else {
            if (dy & 0x7FE00000) {
                lookup = dy / (dx >> 10);
            } else {
                lookup = (dy << 10) / dx;
            }
            ret = 0x400 - D_8009A6EC[lookup];
        }
        if (flag0) {
            ret = 0x800 - ret;
        }
        if (flag1) {
            ret = -ret;
        }
        return ret;
    }
    return 0;
}
