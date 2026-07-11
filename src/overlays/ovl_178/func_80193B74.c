typedef int s32;

extern char D_80198A1C;
extern char D_801989E4;
extern char D_80198A38;

s32 func_800C251C(void *arg0, void *arg1);
s32 func_800C2758(void *arg0, void *arg1, void *arg2);
void func_80193BDC(void *arg0);

s32 func_80193B74(void *arg0) {
    if ((func_800C251C(arg0, &D_80198A1C) | func_800C2758(arg0, &D_801989E4, &D_80198A38)) == -1) {
        func_80193BDC(arg0);
    }

    return 0;
}
