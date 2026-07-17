/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
typedef signed char s8;typedef unsigned char u8;typedef short s16;typedef unsigned short u16;typedef int s32;typedef unsigned int u32;typedef long long s64;
#define NULL ((void *)0)
extern s16 *D_8009D048;
extern s32 D_8009D050;

void *Item_LookupBaseData(unsigned int index);
s32 Inv_FindSlotByIndex();
s32 Inv_WriteSlotById();

s32 Inv_CheckSlotUsable(s32 arg0) {
    s32 ret;
    s32 slot;
    s32 t;
    s16 *cur;
    s16 *end;
    u8 *ent;

    ret = 0;
    cur = D_8009D048;
    end = D_8009D048 + D_8009D050;
    if (cur < end) {
        do {
            if (*cur == 0) {
                break;
            }
            cur++;
        } while (cur < end);
        if (cur < D_8009D048 + D_8009D050) {
            t = cur - D_8009D048;
        } else {
            t = -1;
        }
    } else {
        t = -1;
    }
    slot = t;
    if (arg0 < 0x100) {
        ent = Item_LookupBaseData(arg0 - 1);
        if (ent == NULL) {
            return ret;
        }
        switch (ent[6]) {
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
            ret = Inv_FindSlotByIndex(arg0) == 0;
            break;
        case 10:
        case 12:
        case 13:
        case 14:
        case 15:
            if (slot >= 0) {
                D_8009D048[slot] = arg0;
            } else {
                ret = 1;
            }
            break;
        case 16:
        case 17:
        case 18:
            ret = Inv_WriteSlotById(ent);
            break;
        }
    } else {
        if (slot >= 0) {
            D_8009D048[slot] = arg0;
        } else {
            ret = 1;
        }
    }
    return ret;
}
