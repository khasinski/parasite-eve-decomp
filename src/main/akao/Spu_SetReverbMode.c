/* MASPSX_FLAGS: --store-jump-delay --store-return-delay */

extern int D_8009B38C;
extern int D_8009B418;

int Spu_SetReverbMode(int arg0) {
    int ret;

    switch (arg0) {
    case 0:
        ret = 0;
        break;
    case 1:
        ret = 1;
        break;
    default:
        ret = 0;
        break;
    }

    D_8009B38C = arg0;
    D_8009B418 = ret;
    return ret;
}
