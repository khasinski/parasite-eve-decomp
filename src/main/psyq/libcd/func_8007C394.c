/* MASPSX_FLAGS: --store-branch-delay --store-return-delay */

typedef signed char s8;
typedef unsigned char u8;
typedef short s16;
typedef unsigned short u16;

extern u8 * volatile D_800C0DC8;
extern int D_800C20C4;
extern int D_800BE9EC;

typedef struct StRingEntry {
    s16 status;
    u8 pad_02[4];
    u16 count;
    u8 pad_08[0x18];
} StRingEntry;

int func_8007C394(u8 *ptr) {
    register u8 *base asm("$3");
    int index;
    register int i asm("$4");
    register int count asm("$2");
    register int limit asm("$6");
    register StRingEntry *entry asm("$3");
    register int status asm("$2");
    register int raw_count asm("$3");
    register int expected_status asm("$4");
    register int next_index asm("$2");

    base = D_800C0DC8;
    index = ((ptr - (base + (D_800C20C4 << 5))) >> 2) / 504;
    entry = (StRingEntry *)(base + (index << 5));
    /* Keep the status literal from scheduling before the entry address math. */
    asm volatile("" : : "r"(entry));

    expected_status = 4;
    status = entry->status;
    raw_count = entry->count;
    if (status != expected_status) {
        return 1;
    }

    count = (s16)raw_count;
    i = 0;
    if (count > 0) {
        limit = count;
        do {
            int slot = i + index;
            i++;
            ((StRingEntry *)(D_800C0DC8 + (slot << 5)))->status = 0;
        } while (i < limit);
    }

    next_index = i + index;
    D_800BE9EC = next_index;
    return 0;
}
