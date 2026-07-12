extern char D_800E0F28[];
extern void FieldEng_Register(void *o, void *table);

int func_800CD8C8(void *o) {
    FieldEng_Register(o, D_800E0F28);
    return 0;
}
