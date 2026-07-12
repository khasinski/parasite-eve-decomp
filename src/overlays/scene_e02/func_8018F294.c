extern int func_800C6CE0(void);
extern void func_800C2414(void *arg0, void *arg1);
extern char D_80195488[];

int func_8018F294(void *arg0) {
    int ret = 0;

    if (func_800C6CE0() == 3) {
        func_800C2414(arg0, D_80195488);
        ret = 0;
    }

    return ret;
}
