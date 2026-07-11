/* MASPSX_FLAGS: --store-return-delay */

extern int D_8009B38C;
extern int D_8009B418;

int Spu_SetReverbMode(int mode) {
    int enabled;

    switch (mode) {
    case 0:
        enabled = 0;
        break;
    case 1:
        enabled = 1;
        break;
    default:
        enabled = 0;
        break;
    }

    D_8009B38C = mode;
    D_8009B418 = enabled;
    return enabled;
}
