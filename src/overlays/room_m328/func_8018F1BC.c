typedef unsigned int u32;

extern int FieldEng_GetStatus(void);

int func_8018F1BC(char *arg0) {
    u32 **node;

    arg0[0] = 4;
    if ((unsigned int)FieldEng_GetStatus() >= 2) {
        node = *(u32 ***)(arg0 + 8);
        (*node)[0] &= 0xC0FFFFFF;
    }
    return 0;
}
