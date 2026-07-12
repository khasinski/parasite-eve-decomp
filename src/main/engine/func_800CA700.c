extern char D_800E0B84[];
extern void FieldEng_Register(void *o, void *table);

int func_800CA700(void *o) {
    FieldEng_Register(o, D_800E0B84);
    return 0;
}
