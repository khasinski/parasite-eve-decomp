/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
typedef signed char s8;typedef unsigned char u8;typedef short s16;typedef unsigned short u16;typedef int s32;typedef unsigned int u32;typedef long long s64;
#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"
M2C_UNK qsort();
extern s32 D_8009D0A0;
extern s32 D_8009D0A8;
extern u16 *g_InvSortListBase;
extern s32 g_InvSortListCount;
extern void *(*g_InvLookupPtr)(s16);
extern M2C_UNK Inv_CompareItemsByStatForSort[];
#define Inv_CompareItemsByStatForSort (Inv_CompareItemsByStatForSort[0])

void Inv_SortAmmoSubrange(void) {
    u16 *var_s0;
    u16 *var_s1;
    s32 mask;
    s32 var_a0;
    u16 tmpu;

    var_s0 = g_InvSortListBase;
    D_8009D0A0 = D_8009D0A8;
    mask = 0x200;
    if (var_s0 < g_InvSortListBase + g_InvSortListCount) {
        while (*(s16 *) var_s0 != 0) {
            if ((mask >> M2C_FIELD(g_InvLookupPtr((s16) *var_s0), u8 *, 6)) & 1) {
                break;
            }
            var_s0 += 1;
            if (!(var_s0 < g_InvSortListBase + g_InvSortListCount)) {
                return;
            }
        }
        if (var_s0 < g_InvSortListBase + g_InvSortListCount) {
            tmpu = *var_s0;
            var_s1 = var_s0;
            if (*(s16 *) var_s0 != 0) {
                do {
                    var_a0 = tmpu << 0x10;
                    if (!((mask >> M2C_FIELD(g_InvLookupPtr((s16) (var_a0 >> 0x10)), u8 *, 6)) & 1)) {
                        break;
                    }
                    var_s0 += 1;
                    if (!(var_s0 < g_InvSortListBase + g_InvSortListCount)) {
                        break;
                    }
                    tmpu = *var_s0;
                } while (*(s16 *) var_s0 != 0);
                qsort(var_s1, (s32) (var_s0 - var_s1), 2, &Inv_CompareItemsByStatForSort);
            }
        }
    }
}
