typedef signed short s16;

void *func_800CE610(char *list) {
    char frame[8];
    register char *base asm("$8");
    register char *entry asm("$4");
    register int slot_or_count asm("$5");
    register int count asm("$6");
    register int stride asm("$7");
    register int i asm("$3");
    register void *ret asm("$2");

    (void)frame;
    ret = list;
    asm volatile("" : "=r"(ret) : "0"(ret));
    base = list;
    asm volatile("" : "=r"(base) : "0"(base));
    entry = list + 0xC;
    asm volatile("" : "=r"(entry) : "0"(entry));
    slot_or_count = *(int *)(ret + 0x4);
    stride = *(int *)(ret + 0x0);
    i = 0;
    if (slot_or_count > 0) {
        count = slot_or_count;
loop:
        slot_or_count = (int)entry;
        if (*(s16 *)entry == 0) {
            goto found;
        }
        i++;
        entry += stride;
        if (i < count) {
            goto loop;
        }

found:
        ret = (void *)(i < *(int *)(base + 0x4));
        if (ret != 0) {
            ret = entry + 0x4;
            *(s16 *)slot_or_count = 1;
            *(s16 *)(slot_or_count + 0x2) = 0;
            return ret;
        }
    }

    return 0;
}
