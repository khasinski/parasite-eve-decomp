extern unsigned char D_80091A1F[];
extern unsigned char D_80091A1F_rd[] __asm__("D_80091A1F");
extern unsigned char *D_80091A28;

int Menu_FindSelectedEquipSlotItem(void) {
    unsigned char *hdr;
    unsigned char *p;
    unsigned char *p2;
    unsigned char *q;
    unsigned char *t;
    int i;
    int found;
    int slot;

    found = 0;
    hdr = D_80091A28;
    p = hdr + 1;
    for (i = 0; i < hdr[3]; i++) {
        q = p + i;
        if (q[3] == 3) {
            found = i;
            i = p[2];
        }
    }
    p2 = p + 0x1B;
    for (i = 0; i < p[0x1B]; i++) {
        q = p2 + i;
        if (q[1] == (found & 0xFF)) {
            slot = i;
            goto store;
        }
    }
    slot = 0xFF;
store:
    D_80091A1F[0] = slot;
    __asm__ volatile("" : : : "memory");
    t = D_80091A28;
    return *(t + *(t + D_80091A1F_rd[0] + 0x1D) + 4);
}
