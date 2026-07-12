extern char D_800E0E60[];
extern void FieldEng_Register(void *o, void *table);

int func_800CCEE8(void *o) {
    FieldEng_Register(o, D_800E0E60);
    return 0;
}
