typedef int s32;
typedef unsigned char u8;

extern u8 D_8019A8C8[];

extern s32 func_80052B2C(void);

s32 func_80193114(s32 arg0, s32 **arg1) {
    s32 variant;
    s32 threshold;
    s32 i;
    s32 sum;
    u8 *weights;

    variant = (*arg1[0] >> 10) & 3;
    arg1 = (s32 **)*arg1[1];

    if ((s32)arg1 < 0x280) {
        arg1 = (s32 **)0;
    } else if ((s32)arg1 < 0x600) {
        arg1 = (s32 **)1;
    } else {
        arg1 = (s32 **)2;
    }

    weights = D_8019A8C8 + (arg0 * 84) + ((s32)arg1 * 28) + (variant * 7);
    threshold = ((func_80052B2C() & 0xFF) * 100) / 0x100;

    i = 0;
    sum = 0;
    while (i < 6) {
        sum += weights[i];
        if (threshold < sum) {
            return i;
        }
        i++;
    }

    return i;
}
