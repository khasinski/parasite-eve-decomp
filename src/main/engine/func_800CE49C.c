extern void *D_800E1044[];
extern char D_800C2244[];
extern int printf(char *fmt, ...);

int func_800CE49C(char *obj, int index) {
    char *entry = D_800E1044[index];

    if (entry != 0) {
        *(char **)(obj + 0x8C) = entry;
        *(int *)(obj + 0xC) = *(int *)(entry + 0x34);
        return 0;
    }

    printf(D_800C2244);
    return -1;
}
