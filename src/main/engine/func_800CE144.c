extern char D_800E0FC0[];
extern void FieldEng_Register(void *o, void *table);

int func_800CE144(void *o) {
    FieldEng_Register(o, D_800E0FC0);
    return 0;
}
