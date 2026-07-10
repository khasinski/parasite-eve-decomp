typedef signed char s8;
typedef signed short s16;
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

#define U8(base, off) (*(u8 *)((u8 *)(base) + (off)))
#define S16(base, off) (*(s16 *)((u8 *)(base) + (off)))
#define U16(base, off) (*(u16 *)((u8 *)(base) + (off)))
#define U32(base, off) (*(u32 *)((u8 *)(base) + (off)))
#define COPY32(dst, src)                 \
    do {                                 \
        int _i;                          \
        for (_i = 0; _i < 0x20; _i++) {  \
            ((u8 *)(dst))[_i] = ((u8 *)(src))[_i]; \
        }                                \
    } while (0)
#define RESOLVE_ITEM_DATA(out, item_id)                                      \
    do {                                                                     \
        int _item_id = (item_id);                                             \
        if ((unsigned int)(_item_id - 0x100) < 0x80) {                        \
            (out) = D_800BEEAC + (_item_id * 0x20);                           \
        } else if ((unsigned int)(_item_id - 1) < 0xFF) {                     \
            (out) = Item_LookupBaseData(_item_id - 1);                        \
        } else if ((unsigned int)(_item_id - 0x200) < 9) {                    \
            (out) = D_8009DE64 + (_item_id * 0x20);                           \
        } else {                                                             \
            (out) = 0;                                                        \
        }                                                                    \
    } while (0)

extern void *D_8009D070;
extern void *D_8009D074;
extern s16 *D_8009D048;
extern int D_8009D050;
extern u8 D_800A1E44[];
extern u8 D_800A1F94[];
extern s8 D_800C0E20;
extern u8 D_800BEEAC[];
extern u8 D_8009DE64[];

u32 *BattleCmd_AllocSlot(void);
u8 *Item_LookupBaseData(int index);
void Inv_SetActiveList(int mode, void **selected);

void Inv_StepScrollDisplay(void) {
    u32 *cmd;
    u8 *left_item;
    u8 *right_item;
    u8 *item;
    void *selected;
    int amount;
    int base_total;
    int new_total;

    if (U16(D_8009D070, 0xA) == U16(D_800A1F94, 0xA) &&
        U16(D_8009D074, 0xA) == U16(D_800A1F94 + 0x20, 0xA)) {
        return;
    }

    cmd = BattleCmd_AllocSlot();
    cmd[0] = 4;
    cmd[1] = (u32)D_8009D070;
    cmd[2] = (u32)D_8009D074;
    cmd[3] = 0;

    if (U16(D_800A1F94, 0xC) != 0) {
        left_item = D_800A1E44 + (U8(D_800A1F94, 0x1F) * 0x20);
        cmd[3] = (u32)left_item;
        cmd[8] = U16(left_item, 0xA);

        base_total = U8(left_item, 9) + S16(left_item, 0x12);
        amount = U16(left_item, 0xA) + U16(D_800A1F94, 0xC);
        if ((base_total < 1000 && base_total < amount) ||
            (base_total >= 1000 && amount >= 1000)) {
            new_total = U8(left_item, 9) + S16(left_item, 0x12);
            if (new_total >= 1000) {
                new_total = 999;
            }
        } else {
            new_total = amount;
        }
        U16(left_item, 0xA) = new_total;
    } else if (U16(D_800A1F94 + 0x20, 0xC) != 0) {
        right_item = D_800A1E44 + (U8(D_800A1F94 + 0x20, 0x1F) * 0x20);
        cmd[3] = (u32)right_item;
        cmd[8] = U16(right_item, 0xA);

        base_total = U8(right_item, 9) + S16(right_item, 0x12);
        amount = U16(right_item, 0xA) + U16(D_800A1F94 + 0x20, 0xC);
        if ((base_total < 1000 && base_total < amount) ||
            (base_total >= 1000 && amount >= 1000)) {
            new_total = U8(right_item, 9) + S16(right_item, 0x12);
            if (new_total >= 1000) {
                new_total = 999;
            }
        } else {
            new_total = amount;
        }
        U16(right_item, 0xA) = new_total;
    }

    cmd[5] = U16(D_8009D070, 0xA);
    cmd[7] = U16(D_8009D074, 0xA);

    COPY32(D_8009D070, D_800A1F94);
    COPY32(D_8009D074, D_800A1F94 + 0x20);

    if (D_800C0E20 >= 0 && D_800C0E20 < D_8009D050) {
        RESOLVE_ITEM_DATA(item, D_8009D048[D_800C0E20]);
    } else {
        item = 0;
    }

    selected = item;
    if (item == D_8009D070) {
        Inv_SetActiveList(5, &selected);
    } else if (item == D_8009D074) {
        Inv_SetActiveList(5, &selected);
    } else {
        Inv_SetActiveList(7, &selected);
    }
}
