void func_80192690(char *obj) {
    extern char D_801926B4[];

    if (*(unsigned char *)(*(int *)(obj + 8) + 0xE) == 0x10) {
        __asm__ volatile("" ::: "memory");
        *(void **)(obj + 0xC) = D_801926B4;
    }
}
