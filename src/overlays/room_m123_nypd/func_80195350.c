extern void *D_800E2368;
extern char D_80195684[];

void *func_80195350(int phase) {
    if (phase == 1) {
        *(short *)((char *)D_800E2368 + 0x12) = 1;
    }

    return D_80195684;
}
