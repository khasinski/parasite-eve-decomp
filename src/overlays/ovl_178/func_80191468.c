typedef int s32;

extern char D_801988D0;
extern char D_80198878;
extern char D_801988FC;

s32 func_800C251C(void *arg0, void *arg1);
s32 func_800C2758(void *arg0, void *arg1, void *arg2);
void func_801914D0(void *arg0);

s32 func_80191468(void *arg0) {
    if ((func_800C251C(arg0, &D_801988D0) | func_800C2758(arg0, &D_80198878, &D_801988FC)) == -1) {
        func_801914D0(arg0);
    }

    return 0;
}
