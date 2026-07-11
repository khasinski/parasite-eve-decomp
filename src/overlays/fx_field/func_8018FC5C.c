extern unsigned int D_8018FCEC;
extern unsigned int D_8018FCF0;
extern unsigned int D_8018FCF4;

unsigned int *func_8018FC5C(unsigned int arg0, unsigned int arg1, unsigned int arg2, unsigned int arg3) {
    unsigned int *slot = &D_8018FCEC;

    *slot = arg1;
    D_8018FCF0 = arg2;
    D_8018FCF4 = arg3;
    return slot;
}
