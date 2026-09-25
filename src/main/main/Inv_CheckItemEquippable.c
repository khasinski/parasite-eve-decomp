/* CC1_FLAGS: -G8 -fno-schedule-insns */
/* MASPSX_FLAGS: -G8 */
#include "common.h"
extern s16 *D_8009D048;
extern int D_8009D050;
extern u8 D_800BEEAC[];
extern u8 D_8009DE64[];
extern s16 D_800C0E48[];
extern struct { char _[16]; } D_800C0E22_obj __asm__("D_800C0E22");

#define D_800C0E22 (*(s8 *)&D_800C0E22_obj)

int Inv_RestoreSelection(unsigned int index);
void *Item_LookupBaseData(unsigned int index);
int Inv_CheckFreeSlotCapacity(int mask);

int Inv_CheckItemEquippable(unsigned int list_index, int modifier_index) {
    /* Match the retail 0x60-byte stack frame; remove with a cleaner TU model. */
    volatile unsigned char stack_pad[32];
    register int mod_index asm("$18") = modifier_index;
    int selected_a;
    int selected_b;
    register u8 *item_a asm("$17");
    u8 *item_b;
    int item_id;
    register int count asm("$5");
    int i;
    register int result asm("$19");
    int mask;
    register int modifier asm("$4");
    int existing;
    int different_slot;

    mask = 0;
    selected_a = Inv_RestoreSelection(list_index);
    item_a = 0;
    if (selected_a >= 0 && selected_a < D_8009D050) {
        register u8 *resolved asm("$2");
        register int saved_id asm("$5");
        register int scaled_id asm("$3");
        item_id = D_8009D048[selected_a];
        saved_id = item_id;
        if ((unsigned int)(item_id - 0x100) < 0x80) {
            resolved = D_800BEEAC + (item_id << 5);
        } else if ((unsigned int)(item_id - 1) < 0xFF) {
            resolved = Item_LookupBaseData(item_id - 1);
        } else if ((unsigned int)(saved_id - 0x200) < 9) {
            scaled_id = saved_id << 5;
            resolved = D_8009DE64 + scaled_id;
        } else {
            resolved = 0;
        }
        item_a = resolved;
    }

    selected_b = Inv_RestoreSelection(list_index < 1);
    item_b = 0;
    if (selected_b >= 0 && selected_b < D_8009D050) {
        register u8 *resolved asm("$2");
        register int saved_id asm("$5");
        register int scaled_id asm("$3");
        item_id = D_8009D048[selected_b];
        saved_id = item_id;
        if ((unsigned int)(item_id - 0x100) < 0x80) {
            scaled_id = item_id << 5;
            resolved = D_800BEEAC + scaled_id;
        } else if ((unsigned int)(item_id - 1) < 0xFF) {
            resolved = Item_LookupBaseData(item_id - 1);
        } else if ((unsigned int)(saved_id - 0x200) < 9) {
            scaled_id = saved_id << 5;
            resolved = D_8009DE64 + scaled_id;
        } else {
            resolved = 0;
        }
        item_b = resolved;
    }

    result = 1;
    if (mod_index >= 0) {
        u8 *chosen = item_a + mod_index;
        count = item_b[0x14];
        asm volatile("" : "=r"(count) : "0"(count));
        modifier = chosen[0x15];
        i = 0;
        if (count <= 0) goto first_after;
first_loop:
        if ((item_b + i)[0x15] == modifier) goto first_found;
        ++i;
        if (i < count) goto first_loop;
first_found:
        if (i < item_b[0x14]) return 4;
first_after:;

        for (i = 0; i < item_b[0x14]; i++) {
            if ((item_b + i)[0x15] == 0) {
                break;
            }
        }

        if (i >= item_b[0x14]) {
            modifier = (item_a + mod_index)[0x15] & 0xE0;
            if (modifier == 0) {
                return 3;
            }

            {
                int third_count;
                register int third_modifier asm("$5");
                register int third_loop_count asm("$4");
                third_count = item_b[0x14];
                if (third_count <= 0) goto third_return3;
                i = 0;
                third_modifier = modifier;
                third_loop_count = third_count;
third_loop:
                if (((item_b + i)[0x15] & 0xE0) == third_modifier) goto third_found;
                ++i;
                if (i < third_loop_count) goto third_loop;
third_found:
                if (i < item_b[0x14]) goto third_done;
third_return3:
                return 3;
third_done:;
            }
        }
    }

    Inv_RestoreSelection(list_index);
    if (D_8009D048 == D_800C0E48 && selected_a == D_800C0E22) {
        {
            register int raw_count asm("$2");
            register int loop_count asm("$4");
            register int slot asm("$5");
            raw_count = item_a[0x14];
            i = 0;
            if (raw_count <= 0) goto equip_after;
            loop_count = raw_count;
equip_loop:
            raw_count = (item_a + i)[0x15];
            raw_count &= 0x1F;
            slot = raw_count - 8;
            if ((unsigned int)slot < 3) goto equip_found;
            ++i;
            if (i < loop_count) goto equip_loop;
equip_found:
            if (i < item_a[0x14]) {
                register int equip_mask asm("$4") = 1 << slot;
                different_slot = i != mod_index;
                result = Inv_CheckFreeSlotCapacity(equip_mask);
                if (result != 0) goto return_result;
                if (different_slot != 0 || mod_index < 0) result = 5;
            }
equip_after:;
        }
        return result;
    }

    Inv_RestoreSelection(list_index < 1);
    if (D_8009D048 != D_800C0E48) {
        goto return_one;
    }
    if (selected_b != D_800C0E22) {
        goto return_one;
    }
    if (mod_index < 0) {
        goto return_one;
    }

    {
        register int raw asm("$2");
        register int final_mod asm("$5");
        register int final_count asm("$6");
        register int final_existing asm("$4");
        register int bit_existing asm("$3");
        raw = (item_a + mod_index)[0x15];
        asm volatile("" : : "r"(item_a), "r"(mod_index));
        raw &= 0x1F;
        final_mod = raw - 8;
        if ((unsigned int)final_mod >= 3) goto return_one;
        raw = item_b[0x14];
        i = 0;
        if (raw <= 0) goto final_call;
        final_count = raw;
final_loop:
        raw = (item_b + i)[0x15];
        raw &= 0x1F;
        final_existing = raw - 8;
        if ((unsigned int)final_existing < 3) goto final_found;
        ++i;
        if (i < final_count) goto final_loop;
final_found:
        if (i >= item_b[0x14]) goto final_call;
        bit_existing = 1 << final_existing;
        mask = bit_existing - (1 << final_mod);
    }
final_call:
    result = Inv_CheckFreeSlotCapacity(mask);
    goto return_result;
return_one:
    return 1;
return_result:
    return result;
}
