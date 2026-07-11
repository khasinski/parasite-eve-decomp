/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

typedef unsigned char u8;
typedef short s16;
typedef int s32;

extern s32 D_8009D0A0;
extern u8 *(*g_InvLookupPtr)(s16) __asm__("D_8009D0B4");

int Inv_CompareItemsByStatForSort(s16 *lhs, s16 *rhs) {
    u8 *lhs_data;
    u8 *rhs_data;
    int lhs_value;
    int rhs_value;
    int lhs_name;
    int rhs_name;

    lhs_data = g_InvLookupPtr(*lhs);
    rhs_data = g_InvLookupPtr(*rhs);

    lhs_value = 0;
    rhs_value = 0;
    switch (D_8009D0A0) {
    case 0:
        lhs_value = lhs_data[7] + *(s16 *)(lhs_data + 0xE);
        rhs_value = rhs_data[7] + *(s16 *)(rhs_data + 0xE);
        break;
    case 1:
        lhs_value = lhs_data[8] + *(s16 *)(lhs_data + 0x10);
        rhs_value = rhs_data[8] + *(s16 *)(rhs_data + 0x10);
        break;
    case 2:
        lhs_value = lhs_data[9] + *(s16 *)(lhs_data + 0x12);
        rhs_value = rhs_data[9] + *(s16 *)(rhs_data + 0x12);
        break;
    }

    if (rhs_value < lhs_value) {
        return -1;
    }
    if (lhs_value < rhs_value) {
        return 1;
    }

    lhs_name = lhs_data[4];
    rhs_name = rhs_data[4];
    if ((unsigned int)rhs_name < (unsigned int)lhs_name) {
        return 1;
    }
    return -((unsigned int)lhs_name < (unsigned int)rhs_name);
}
