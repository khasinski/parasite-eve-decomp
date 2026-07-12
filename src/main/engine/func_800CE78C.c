typedef signed short s16;

extern int D_800E27EC;
extern char *D_800E2368;

int func_800CE78C(char *list) {
    int old_context = D_800E27EC;
    char *entry = list + 0xC;
    char *payload;
    int i = 0;
    int active = 0;
    int stride = *(int *)(list + 0x0);
    int count = *(int *)(list + 0x4);
    int (*callback)(int, void *, int) = *(int (**)(int, void *, int))(list + 0x8);

    if (count > 0) {
        payload = list + 0x10;
        do {
            if (*(s16 *)entry != 0) {
                active++;
                D_800E27EC = *(s16 *)(payload - 0x2);
                callback(2, payload, *(int *)(D_800E2368 + 0x8));
            }

            i++;
            payload += stride;
            entry += stride;
        } while (i < *(int *)(list + 0x4));
    }

    D_800E27EC = old_context;
    return active;
}
