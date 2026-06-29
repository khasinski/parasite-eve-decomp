/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

typedef unsigned char u8;
typedef unsigned short u16;

int g_InvCategoryBaseItemId;
u16 *g_InvItemPtr;
int g_InvSlotLimit;
extern u16 g_InvCategoryItemTable[];
extern u8 g_ItemCodeRemapTable[];

int Inv_CountByValue(int arg0) {
    int count;
    u16 *ptr;
    register u16 *end asm("$3");
    register u16 *iter_end asm("$7");
    int temp;
    int value;

    count = 0;
    if ((arg0 >= g_InvCategoryBaseItemId) && (arg0 < g_InvCategoryBaseItemId + 3)) {
        count = g_InvCategoryItemTable[(arg0 - g_InvCategoryBaseItemId) * 0x10];
    } else {
        temp = g_InvSlotLimit;
        ptr = g_InvItemPtr;
        temp <<= 1;
        end = (u16 *)((u8 *)ptr + temp);
        if (ptr < end) {
            iter_end = end;
            do {
                value = *ptr;
                if ((unsigned int)(value - 0x100) < 0x80) {
                    temp = g_ItemCodeRemapTable[(value << 16) >> 11];
                } else {
                    temp = (value << 16) >> 16;
                }
                count += ((temp ^ arg0) < 1U);
                ptr++;
            } while (ptr < iter_end);
        }
    }

    return count;
}
