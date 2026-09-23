/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
#include "pe1/inventory_slots.h"
void Inv_SelectActiveList(int);
extern u8 D_800923D0[];
typedef struct WeaponListOutput {
    u16 attack;
    u16 range;
    u16 unknown04;
    u16 kind;
    u32 unknown08;
    u32 packed;
    u32 effects;
} WeaponListOutput;
static inline int ClampStat(ItemDataRecord *item, int which) {
    int value = item->baseStats[which] + item->bonusStats[which];
    int result = 999;
    if (value < 1000) result = value;
    return result;
}
void Inv_BuildWeaponList(int unused, WeaponListOutput *out) {
    ItemDataRecord *item;
    /* Preserve the eight-byte local area in the retail stack frame. */
    int frame_padding[2];
    int value, i, kind, code, root, shift, trial, radicand;
    unsigned int packed;
    int product;
    Inv_SelectActiveList(0);
    item = Inv_LookupActiveListData(D_800C0E20.tracked[0]);
    {
        int bonus = item->bonusStats[0];
        int base = item->baseStats[0];
        register int saved asm("$3") = bonus;
        asm("" : "=r"(saved) : "0"(saved));
        if (base + bonus >= 1000) bonus = 999;
        else bonus = saved + base;
        out->attack = bonus;
    }
    {
        int base2 = item->baseStats[1];
        int bonus2 = item->bonusStats[1];
        if (base2 + bonus2 < 1000) product = (base2 + bonus2) * 22500;
        else product = 0x156fabc;
        radicand = product;
    }
    root = 0;
    for (shift = 30; shift >= 0; shift -= 2) {
        trial = ((root << 2) + 1) << shift;
        root <<= 1;
        if (radicand >= trial) { radicand -= trial; root |= 1; }
    }
    packed = out->packed;
    out->range = root;
    out->kind = item->kind;
    packed &= ~0x3ffu;
    packed |= item->ammo & 0x3ff;
    out->packed = packed;
    value = ClampStat(item, 2);
    out->packed = (packed & 0xfff003ffu) | ((value & 0x3ff) << 10);
    kind = item->kind;
    if (kind != 0 && (unsigned)kind < 8) {
        asm volatile("" ::: "memory");
        code = kind - 4;
        if (code <= 0) code = 1;
    } else if ((unsigned)kind < 19) code = 0;
    else code = kind - 18;
    out->effects = 0x11;
    out->packed = (out->packed & 0xffcfffffu) | ((code & 3) << 20);
    for (i = 0; i < item->tailCount; i++) {
        int effect = item->tailData[i] & 31;
        switch (effect) {
        case 1:case 2:case 3:case 4:case 5:
            {
                register u32 mask asm("$4") = -16;
                u32 flags = out->effects;
                register u32 bits asm("$3") = D_800923D0[effect];
                out->effects = (flags & mask) | (bits & 15);
            }
            break;
        case 6:case 7:case 8:
            out->effects=(out->effects&~0xc0)|(((effect-5)&3)<<6);break;
        case 9:out->effects|=0x100;break;
        case 10:out->effects|=0x200;break;
        case 11:out->effects|=0x400;break;
        case 12:out->effects|=0x800;break;
        case 13:out->effects|=0x1000;break;
        case 14:out->effects|=0x8000;break;
        case 15:out->effects|=0x10000;break;
        case 16:case 17:out->effects=(out->effects&~0x6000)|(((effect-15)&3)<<13);break;
        case 18:out->effects|=0x20000;break;
        case 19:case 20:out->effects=(out->effects&~0x30)|(((effect-17)&3)<<4);break;
        }
    }
    if ((out->effects & 0x6000) == 0x2000) out->attack = (s16)out->attack >> 1;
}
