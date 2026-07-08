#include "pe1/inventory.h"

int Spend_Ammo(int amount) {
    InvItemSlot *src;
    InvItemSlot *dst;
    int new_src;
    int new_dst;
    int max;
    int ret;

    ret = 0;
    if (amount > 0) {
        src = &g_InvCompareSlotLeft;
        dst = &g_InvCompareSlotRight;
    } else {
        src = &g_InvCompareSlotRight;
        dst = &g_InvCompareSlotLeft;
        amount = -amount;
    }

    if (src != 0 && dst != 0) {
        new_src = src->ammo - amount;
        new_dst = dst->ammo + amount;
        if (new_src < 0) {
            new_dst += new_src;
            new_src = 0;
            ret = 1;
        }

        max = dst->count_bonus + dst->max_bonus;
        if (max >= 1000) {
            max = 999;
        }
        if (max < new_dst) {
            max = dst->count_bonus + dst->max_bonus;
            if (max >= 1000) {
                max = 999;
            }
            new_src += new_dst - max;
            new_dst = dst->count_bonus + dst->max_bonus;
            ret = 2;
            if (new_dst >= 1000) {
                new_dst = 999;
            }
        }

        src->ammo = new_src;
        dst->ammo = new_dst;
        if (new_src == 0 && src->reserve_ammo != 0) {
            src->ammo = src->reserve_ammo;
            src->reserve_ammo = 0;
        }
    } else {
        ret = 3;
    }

    return ret;
}
