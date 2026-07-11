/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

typedef unsigned char u8;
typedef short s16;
typedef int s32;

extern u8 *(*g_InvLookupPtr)(s16) __asm__("D_8009D0B4");
extern u8 g_InvSortRankTable[] __asm__("D_8009D0B8");
extern u8 g_InvSortTypeRankTable[] __asm__("D_8009D0BC");

void *Inv_LookupActiveListData(int index);

int Inv_CompareItemsForSort(s16 *lhs, s16 *rhs) {
    s16 lhs_id;
    s16 rhs_id;
    u8 *lhs_data;
    u8 *rhs_data;
    int lhs_rank;
    int rhs_rank;
    int lhs_type_rank;
    int rhs_type_rank;
    int lhs_name;
    int rhs_name;

    lhs_id = *lhs;
    rhs_id = *rhs;

    if (g_InvLookupPtr != (void *)Inv_LookupActiveListData) {
        if (lhs_id == 0) {
            return rhs_id != 0;
        }
        if (rhs_id == 0) {
            return -1;
        }
    }

    lhs_data = g_InvLookupPtr(lhs_id);
    rhs_data = g_InvLookupPtr(rhs_id);

    lhs_rank = g_InvSortRankTable[lhs_data[6]];
    rhs_rank = g_InvSortRankTable[rhs_data[6]];
    lhs_type_rank = g_InvSortTypeRankTable[lhs_data[0xE] & 0xF];
    rhs_type_rank = g_InvSortTypeRankTable[rhs_data[0xE] & 0xF];

    if (lhs_data == 0) {
        if (rhs_data != 0) {
            return 1;
        }
    } else if (rhs_data == 0) {
        return -1;
    }

    if (rhs_rank < lhs_rank) {
        return 1;
    }
    if (lhs_rank < rhs_rank) {
        return -1;
    }
    if (rhs_type_rank < lhs_type_rank) {
        return 1;
    }
    if (lhs_type_rank < rhs_type_rank) {
        return -1;
    }

    lhs_name = lhs_data[4];
    rhs_name = rhs_data[4];
    if ((unsigned int)rhs_name < (unsigned int)lhs_name) {
        return 1;
    }
    return -((unsigned int)lhs_name < (unsigned int)rhs_name);
}
