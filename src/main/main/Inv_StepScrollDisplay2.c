/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

typedef unsigned char u8;
typedef short s16;
typedef int s32;

extern s16 *g_InvItemPtr __asm__("D_8009D048");
extern s32 g_InvSlotLimit __asm__("D_8009D050");
extern s16 *g_InvDisplayListPtr __asm__("D_8009D04C");
extern s32 g_InvDisplayListCount __asm__("D_8009D054");
extern s16 *g_InvAuxListPtr __asm__("D_8009D07C");
extern s32 g_InvAuxListCount __asm__("D_8009D080");
extern u8 g_KeyItemDataTable[] __asm__("D_8009DE64");
extern u8 g_EquipItemDataTable[] __asm__("D_800BEEAC");
extern s16 g_InvAuxListPrimary[] __asm__("D_800C1F80");
extern s16 g_InvAuxListSecondary[] __asm__("D_800C1EB8");

void *Item_LookupBaseData(unsigned int index);

#define LOOKUP_SLOT_DATA(slot_, out_)                              \
    do {                                                          \
        int item_id_;                                             \
        if ((slot_) < 0 || (slot_) >= g_InvSlotLimit) {           \
            (out_) = 0;                                           \
        } else {                                                  \
            item_id_ = g_InvItemPtr[(slot_)];                     \
            if ((unsigned int)(item_id_ - 0x100) < 0x80) {        \
                (out_) = g_EquipItemDataTable + (item_id_ << 5);  \
            } else if ((unsigned int)(item_id_ - 1) < 0xFF) {     \
                (out_) = Item_LookupBaseData(item_id_ - 1);       \
            } else if ((unsigned int)(item_id_ - 0x200) < 9) {    \
                (out_) = g_KeyItemDataTable + (item_id_ << 5);    \
            } else {                                              \
                (out_) = 0;                                       \
            }                                                     \
        }                                                         \
    } while (0)

void Inv_StepScrollDisplay2(int use_primary) {
    int index;
    int has_marker;
    u8 *data;

    if (use_primary != 0) {
        g_InvAuxListPtr = g_InvAuxListPrimary;

        has_marker = 0;
        for (index = 0; index < 0x52; index++) {
            if (g_InvAuxListPtr[index] == 0x204) {
                has_marker = 1;
                break;
            }
        }

        if (!has_marker) {
            for (index = 0; index < g_InvSlotLimit; index++) {
                LOOKUP_SLOT_DATA(index, data);
                if (data != 0 && data[6] == 6) {
                    break;
                }
            }

            if (index < g_InvSlotLimit) {
                for (index = 0; index < 0x51 && g_InvAuxListPtr[index] != 0; index++) {
                }

                if (index < 0x52) {
                    g_InvAuxListPtr[index] = 0x204;
                    has_marker = 1;
                }
            }
        }

        g_InvAuxListCount = has_marker + 0x50;
    } else {
        g_InvAuxListPtr = g_InvAuxListSecondary;
        g_InvAuxListCount = 0x64;
    }

    g_InvDisplayListPtr = g_InvAuxListPtr;
    g_InvDisplayListCount = g_InvAuxListCount;
}
