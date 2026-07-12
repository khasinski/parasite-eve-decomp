extern void *D_800E1044[];
extern char D_800C2244[];
extern int printf(char *fmt, ...);

int func_800CE49C(char *obj, int index) {
    register int offset asm("$2") = index * 4;
    char *entry = *(char **)((char *)D_800E1044 + offset);

    if (entry == 0) {
        printf(D_800C2244);
        return -1;
    }

    *(char **)(obj + 0x8C) = entry;
    *(int *)(obj + 0xC) = *(int *)(entry + 0x34);
    return 0;
}
