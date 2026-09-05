extern unsigned char D_80091A1F[];
extern unsigned char D_80091A1F_rd[] __asm__("D_80091A1F");
extern unsigned char *D_80091A28;

int Menu_FindSelectedEquipSlotItem(void) {
    register unsigned char *base asm("$2");
    unsigned char *list;
    register int selected_slot asm("$6");
    int index;
    int count;
    int found;
    register int result asm("$2");
    register int wanted asm("$5");
    unsigned char *slots;
    register int limit asm("$4");
    unsigned char *t;

    /* Retail reserves 16 unused bytes; the original purpose is unknown. */
    int stack_pad[4];

    selected_slot = 0;
    base = D_80091A28;
    index = 0;
    count = base[3];
    list = base + 1;

    if (count > 0) {
        do {
            if ((list + index)[3] == 3) {
                selected_slot = index;
                index = list[2];
            }

            index++;
        } while (index < count);
        index = 0;
    }

    found = list[0x1B];
    slots = list + 0x1B;
    if (found > 0) {
        wanted = selected_slot & 0xFF;
        limit = found;
        do {
            if ((slots + index)[1] == wanted) {
                result = index;
                goto store_result;
            }

            index++;
        } while (index < limit);
    }

    result = 0xFF;

store_result:
    D_80091A1F[0] = result;
    asm("" : : : "memory");
    t = D_80091A28;
    result = *(t + *(t + D_80091A1F_rd[0] + 0x1D) + 4);
    return result;
}
