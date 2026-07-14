extern short D_8019AF56;
extern short D_8019AF58;
extern short D_8019AF5A;
extern short D_8019AF5C;
extern short D_8019AF64;
extern char D_8019AEFC[];

void *func_8019A5D4(int unused, int value, int flag) {
    D_8019AF56 = value;
    D_8019AF58 = value;
    value++;
    D_8019AF5C = value * 6;
    D_8019AF5A = flag;
    D_8019AF64 = flag != 0 ? 0x52 : 0xE;

    return D_8019AEFC;
}
