typedef int s32;

extern char D_80198694;
extern char D_801986A4;

s32 func_800C251C(void *arg0, void *arg1);
s32 func_800C2758(void *arg0, void *arg1, void *arg2);
void func_8018F338(void *arg0);
void func_80198674(void);

s32 func_8018F2D0(void *arg0) {
    if ((func_800C251C(arg0, &D_80198694) | func_800C2758(arg0, func_80198674, &D_801986A4)) == -1) {
        func_8018F338(arg0);
    }

    return 0;
}
