/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: --use-comm-section -G8 */
#include "include_asm.h"

typedef signed short s16;
typedef unsigned char u8;
typedef unsigned int u32;

signed char g_PlayerMovementEnabled;

int g_TaskCollisionFlag;

signed char g_MenuEquipSlotIndex;

typedef struct ColorTableSlot {
    u8 enabled;
    u8 pad_01[7];
    u8 mode_value;
    u8 mode;
    u8 pad_0A[2];
    u32 flags;
    s16 base;
    s16 x;
    s16 y;
    s16 w;
    s16 h;
    u8 digits[5];
    u8 digit_count;
    u8 pad_20[0x18];
} ColorTableSlot;

extern ColorTableSlot D_800BCEA8[];
extern u8 D_8009CEA0;
extern u8 D_8009CEA4;
extern u8 D_8009CED0;
extern s16 D_8009CE98;
extern s16 D_8009CE9A;
extern s16 D_8009CE9C;
extern s16 D_8009CE9E;

void Task_EnableMovement(void) {
    g_PlayerMovementEnabled = 1;
}

void Task_DisableMovement(void) {
    g_PlayerMovementEnabled = 0;
}

void Task_SetCollisionFlag(int arg0) {
    g_TaskCollisionFlag = arg0 != 0;
}

void Render_SetupColorTable(s16 base, int mode, s16 *src) {
    register s16 *scan asm("$12") = src;
    register int slot_index asm("$7") = 0;
    register int digit_index asm("$9");
    register int one asm("$8") = 1;
    register int neg_one asm("$14") = -1;
    register int div_magic asm("$13") = 0x66666667;
    ColorTableSlot *slot;

    while ((u8)slot_index < 4) {
        slot = &D_800BCEA8[(u8)slot_index];
        if (slot->enabled == 0) {
            u32 flags;

            slot->enabled = one;
            slot->mode = 0;
            slot->base = base;
            D_8009CEA0 = 0;
            D_8009CEA4 = neg_one;
            slot->mode_value = mode;
            flags = slot->flags;
            flags &= 0xFFEFFFFF;
            flags &= 0xFFDFFFFF;
            slot->flags = flags;

            if ((u8)mode != 0) {
                slot->x = D_8009CE98;
                slot->y = D_8009CE9A;
                slot->w = D_8009CE9C;
                slot->h = D_8009CE9E;
                if (slot->mode_value == 3) {
                    slot->flags |= 0x100000;
                    digit_index = 0;
                } else {
                    digit_index = 0;
                }
            } else {
                if (D_8009CED0 != 0) {
                    slot->mode = one;
                }
                digit_index = 0;
            }

            while (digit_index < 5) {
                int value = *scan++;
                int quotient;
                int digit_count;

                if (value == neg_one) {
                    return;
                }

                quotient = value / 10;
                slot->digits[digit_index * 6] = value - quotient * 10;
                digit_count = 0;
                while (quotient != 0) {
                    int next = quotient / 10;
                    digit_count++;
                    slot->digits[digit_index * 6 + digit_count] = quotient - next * 10;
                    quotient = next;
                }
                slot->digits[digit_index * 6 + 5] = digit_count + 1;

                digit_index++;
            }
            return;
        }
        slot_index++;
    }
}
