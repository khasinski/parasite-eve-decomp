typedef int s32;

extern char D_80192B9C;
extern char D_80192B5C;
extern char D_80192BBC;

s32 func_800C251C(void *arg0, void *arg1);
s32 func_800C2758(void *arg0, void *arg1, void *arg2);
void func_8018F12C(void *arg0);

s32 func_8018F0BC(void *arg0) {
    if ((func_800C251C(arg0, &D_80192B9C) | func_800C2758(arg0, &D_80192B5C, &D_80192BBC)) == -1) {
        func_8018F12C(arg0);
    }

    return 0;
}
