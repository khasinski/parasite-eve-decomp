/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

extern unsigned char D_80091A1F[];
extern struct { char _[16]; } D_80091A28_o __asm__("D_80091A28");

#define D_80091A28 (*(unsigned char **)&D_80091A28_o)

int Menu_FindSelectedEquipSlotItem(void) {
    register unsigned char *base asm("$2");
    register unsigned char *list asm("$4");
    register int selected_slot asm("$6");
    register int index asm("$3");
    int count;
    int found;
    register int result asm("$2");
    register int wanted asm("$5");
    register unsigned char *slots asm("$7");
    register int limit asm("$4");
    register int reload asm("$3");

    asm volatile("addiu $sp,$sp,-16" ::: "memory");

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
    asm volatile(
        ".set noat\n\t"
        "lui $at, %%hi(D_80091A1F)\n\t"
        "sb %0, %%lo(D_80091A1F)($at)\n\t"
        "lui %0, %%hi(D_80091A28)\n\t"
        "lw %0, %%lo(D_80091A28)(%0)\n\t"
        "lui %1, %%hi(D_80091A1F)\n\t"
        "lbu %1, %%lo(D_80091A1F)(%1)\n\t"
        "nop\n\t"
        "addu %1, %0, %1\n\t"
        "lbu %1, 0x1D(%1)\n\t"
        "nop\n\t"
        "addu %0, %0, %1\n\t"
        "lbu %0, 0x4(%0)\n\t"
        ".set at"
        : "=r"(result), "=&r"(reload)
        : "0"(result)
        : "memory");

    asm volatile("addiu $sp,$sp,16" ::: "memory");
    return result;
}
