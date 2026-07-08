#ifndef PE1_INVENTORY_H
#define PE1_INVENTORY_H

typedef unsigned char  InvU8;
typedef unsigned short InvU16;
typedef short          InvS16;

typedef struct InvItemSlot {
    InvU8  pad_00[5];
    InvU8  flags;
    InvU8  kind;
    InvU8  pad_07[2];
    InvU8  count_bonus;
    InvU16 ammo;
    InvU16 reserve_ammo;
    InvU8  pad_0E[4];
    InvS16 max_bonus;
    InvU8  pad_14[0x0C];
} InvItemSlot;

extern InvItemSlot g_InvCompareSlotLeft;
extern InvItemSlot g_InvCompareSlotRight;
extern InvItemSlot D_800A1A00;

#endif /* PE1_INVENTORY_H */
