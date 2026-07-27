extern void **FieldEng_GetSlot(void);
extern char D_80194688;

int func_8018F20C(void) {
    *FieldEng_GetSlot() = &D_80194688;
    return 0;
}
