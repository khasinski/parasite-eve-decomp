extern short D_80194160;
extern short D_80194162;
extern short D_80194164;

short *func_80193FF0(int arg0, short arg1, short arg2, short arg3) {
    if (arg0 == 1) {
        D_80194160 = arg1;
        D_80194162 = arg2;
        D_80194164 = arg3;
    }

    return &D_80194160;
}
