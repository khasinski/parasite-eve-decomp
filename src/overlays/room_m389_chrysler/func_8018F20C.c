extern void **FieldEng_GetSlot(void);
extern char D_801941BC;

int func_8018F20C(void) {
    *FieldEng_GetSlot() = &D_801941BC;
    return 0;
}
