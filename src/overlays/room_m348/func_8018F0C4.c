typedef int s32;
typedef unsigned int u32;

extern char D_80192880;

int FieldEng_GetStatus(void);
void func_800C2414(void *arg0, void *arg1);

s32 func_8018F0C4(void *arg0) {
    if (FieldEng_GetStatus() == 3) {
        register u32 *flags asm("$3") = **(u32 ***)((char *)arg0 + 8);
        register u32 mask asm("$5") = 0xBFFFFFFF;

        *flags &= mask;
        func_800C2414(arg0, &D_80192880);
    }

    return 0;
}
