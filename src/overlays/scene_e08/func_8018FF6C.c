typedef int s32;

extern char D_80198740;
extern char D_80198718;
extern char D_80198754;

s32 func_800C251C(void *arg0, void *arg1);
s32 func_800C2758(void *arg0, void *arg1, void *arg2);
void func_8018FFD4(void *arg0);

s32 func_8018FF6C(void *arg0) {
    if ((func_800C251C(arg0, &D_80198740) | func_800C2758(arg0, &D_80198718, &D_80198754)) == -1) {
        func_8018FFD4(arg0);
    }

    return 0;
}
