extern void **FieldEng_GetSlot(void);
extern char D_8019535C;

int func_8018F214(void) {
    *FieldEng_GetSlot() = &D_8019535C;
    return 0;
}
