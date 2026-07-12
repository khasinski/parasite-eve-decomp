extern char D_800E0D08[];
extern void FieldEng_Register(void *o, void *table);

int func_800CBF0C(void *o) {
    FieldEng_Register(o, D_800E0D08);
    return 0;
}
