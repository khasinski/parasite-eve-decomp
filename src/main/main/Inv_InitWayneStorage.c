/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

typedef unsigned char u8;
typedef short s16;
typedef int s32;
typedef unsigned int u32;

extern s16 *g_InvItemPtr __asm__("D_8009D048");
extern s32 g_InvSlotLimit __asm__("D_8009D050");
extern u8 g_KeyItemDataTable[] __asm__("D_8009DE64");
extern u8 g_EquipItemDataTable[] __asm__("D_800BEEAC");
extern u8 D_800A1FE8[] __asm__("D_800A1FE8");
extern u8 D_800A204C[] __asm__("D_800A204C");
extern u8 D_800A206C[] __asm__("D_800A206C");
extern u8 *D_8009D084;
extern s32 D_8009D088;
extern s32 D_8009D08C;

int Inv_RestoreSelection(int index);
void *Item_LookupBaseData(unsigned int index);

#define LOOKUP_SLOT_DATA(slot_, out_)                              \
    do {                                                          \
        int item_id_;                                             \
        int slot_index_;                                          \
        slot_index_ = (slot_);                                    \
        if (slot_index_ < 0 || slot_index_ >= g_InvSlotLimit) {   \
            (out_) = 0;                                           \
        } else {                                                  \
            item_id_ = g_InvItemPtr[slot_index_];                 \
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

#define COPY_ITEM_DATA(dst_, src_)          \
    do {                                    \
        int copy_i_;                        \
        for (copy_i_ = 0; copy_i_ < 0x20; copy_i_++) { \
            (dst_)[copy_i_] = (src_)[copy_i_];         \
        }                                   \
    } while (0)

void Inv_InitWayneStorage(void) {
    int slot;
    u8 *data;

    D_8009D084 = D_800A1FE8;
    D_8009D08C = 0;
    D_8009D088 = 0;

    slot = Inv_RestoreSelection(0);
    LOOKUP_SLOT_DATA(slot, data);
    COPY_ITEM_DATA(D_800A204C, data);

    slot = Inv_RestoreSelection(1);
    LOOKUP_SLOT_DATA(slot, data);
    COPY_ITEM_DATA(D_800A206C, data);
}
