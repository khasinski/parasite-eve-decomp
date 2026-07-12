typedef int s32;
typedef unsigned int u32;

extern char D_80196DA4;

int FieldEng_GetStatus(void);
void FieldEng_Register(void *arg0, void *arg1);

s32 func_801901DC(void *arg0) {
    if (FieldEng_GetStatus() == 3) {
        register u32 *flags asm("$3") = **(u32 ***)((char *)arg0 + 8);
        register u32 mask asm("$5") = 0xBFFFFFFF;

        *flags &= mask;
        FieldEng_Register(arg0, &D_80196DA4);
    }

    return 0;
}
