/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: --use-comm-section -G8 */
#include "include_asm.h"

typedef unsigned char u8;
typedef signed short s16;

int g_InvItemPtr;
int g_InvActiveListOverride;
int g_InvSlotLimit;
int g_InvOverrideSlotLimit;
int g_InvSelectionBits;
int g_InvSelectionBitWords;
extern int g_AyaItemSelectionBits[];
extern short g_AyaInventoryItems[];
extern int g_InvStorageSelectionBits[];
extern int D_8009D03C;
extern int D_8009D04C;
extern s16 D_800C0EAA[];
extern s16 D_800C1F7E[];
extern s16 D_800C2022[];
extern u8 D_800A1E6D[];
extern s16 D_800A1E76[];

typedef struct ItemGridRecord {
    u8 data[0x20];
} ItemGridRecord;

extern ItemGridRecord D_800A1E64[];

int Inv_GetAyaSlotLimit(void);
void Inv_SelectActiveList(int useOverride);
void *Item_LookupBaseData(int item);

void Util_CopyFFTerminatedBytes(u8 *dst, u8 *src)
{
    u8 first;

    first = *src++;
    *dst++ = first;
    if (first != 0xFF) {
        int term;
        int next;

        term = 0xFF;
        do {
            next = *src++;
            *dst++ = next;
        } while (next != term);
    }
}

void Util_AppendFFTerminatedBytes(u8 *dst, u8 *src)
{
    int scan;
    u8 first;

    scan = *dst++;
    if (scan != 0xFF) {
        int term;
        int next;

        term = 0xFF;
        do {
            next = *dst++;
        } while (next != term);
    }

    dst--;

    first = *src++;
    *dst++ = first;
    if (first != 0xFF) {
        int term;
        int next;

        term = 0xFF;
        do {
            next = *src++;
            *dst++ = next;
        } while (next != term);
    }
}

void Inv_BuildItemGridFromCategory(void)
{
    register int i asm("$16");
    register int stride asm("$17");
    register ItemGridRecord *out asm("$18");
    register int div_magic asm("$19");
    register int placeholder asm("$20");
    register s16 *clear asm("$2");
    ItemGridRecord *src;
    int category;
    int base;
    int lookup;

    Inv_SelectActiveList(0);

    i = 0x31;
    clear = D_800C0EAA;
    do {
        *clear = 0;
        i--;
        clear--;
    } while (i >= 0);

    i = 0;
    category = 0x13;
    do {
        lookup = i;
        i++;
        src = Item_LookupBaseData(lookup);
    } while (src != 0 && src->data[6] != category);

    D_8009D03C = i;

    i = 0;
    div_magic = 0x55555556;
    placeholder = 0x3E7;
    out = D_800A1E64;
    stride = 0;
    do {
        register int rem asm("$3");

        asm volatile(
            "mult %0,%1\n"
            "sra $2,%0,31"
            :
            : "r"(i), "r"(div_magic)
            : "$2");
        base = D_8009D03C;
        asm volatile(
            "mfhi $5\n"
            "subu $2,$5,$2\n"
            "sll %0,$2,1\n"
            "addu %0,%0,$2\n"
            "subu %0,%1,%0"
            : "=r"(rem)
            : "r"(i)
            : "$2", "$5");
        base += rem;
        src = Item_LookupBaseData(base - 1);
        *out = *src;

        out++;
        D_800A1E6D[stride] = 0;
        *(s16 *)((u8 *)D_800A1E76 + stride) = placeholder;
        asm volatile("" ::: "memory");
        i++;
        stride += 0x20;
    } while (i < 9);

    i = 0x63;
    clear = D_800C1F7E;
    do {
        *clear = 0;
        i--;
        clear--;
    } while (i >= 0);

    i = 0x51;
    clear = D_800C2022;
    do {
        *clear = 0;
        i--;
        clear--;
    } while (i >= 0);

    D_8009D04C = 0;
}

void Inv_SelectActiveList(int useOverride) {
    if (useOverride != 0 && g_InvActiveListOverride != 0) {
        g_InvItemPtr = g_InvActiveListOverride;
        g_InvSelectionBits = (int)g_InvStorageSelectionBits;
        g_InvSelectionBitWords = 4;
        g_InvSlotLimit = g_InvOverrideSlotLimit;
    } else {
        g_InvItemPtr = (int)g_AyaInventoryItems;
        g_InvSlotLimit = Inv_GetAyaSlotLimit();
        g_InvSelectionBits = (int)g_AyaItemSelectionBits;
        g_InvSelectionBitWords = 2;
    }
}
