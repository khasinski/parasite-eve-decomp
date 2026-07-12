typedef signed short s16;

void *func_800CE610(char *list) {
    char *entry = list + 0xC;
    int count = *(int *)(list + 0x4);
    int stride = *(int *)(list + 0x0);
    int i;

    if (count > 0) {
        i = 0;
        do {
            if (*(s16 *)entry == 0) {
                break;
            }
            i++;
            if (i < count) {
                entry += stride;
            }
        } while (i < count);

        if (i < *(int *)(list + 0x4)) {
            *(s16 *)entry = 1;
            *(s16 *)(entry + 0x2) = 0;
            return entry + 0x4;
        }
    }

    return 0;
}
