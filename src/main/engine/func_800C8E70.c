extern char D_800E09A0[];
extern void FieldEng_Register(void *o, void *table);

int func_800C8E70(void *o) {
    FieldEng_Register(o, D_800E09A0);
    return 0;
}
