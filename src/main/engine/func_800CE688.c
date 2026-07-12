typedef signed short s16;
typedef unsigned short u16;

extern int D_800E27EC;
extern char *D_800E2368;

int func_800CE688(char *list) {
    int old_context = D_800E27EC;
    char *entry = list + 0xC;
    char *timer;
    int i = 0;
    int active = 0;
    int stride = *(int *)(list + 0x0);
    int count = *(int *)(list + 0x4);
    int (*callback)(int, void *, int) = *(int (**)(int, void *, int))(list + 0x8);

    if (count > 0) {
        timer = list + 0xE;
        do {
            if (*(s16 *)entry != 0) {
                active++;
                D_800E27EC = *(s16 *)timer;
                if (callback(1, entry + 0x4, *(int *)(D_800E2368 + 0x8)) != 0) {
                    *(s16 *)entry = 0;
                } else {
                    *(u16 *)timer = *(u16 *)timer + 1;
                }
            }

            i++;
            timer += stride;
            if (i < *(int *)(list + 0x4)) {
                entry += stride;
            }
        } while (i < *(int *)(list + 0x4));
    }

    D_800E27EC = old_context;
    return active;
}
