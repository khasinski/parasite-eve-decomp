extern char D_800E0824[];
extern void FieldEng_Register(void *o, void *table);

int func_800C7D2C(void *o) {
    FieldEng_Register(o, D_800E0824);
    return 0;
}
