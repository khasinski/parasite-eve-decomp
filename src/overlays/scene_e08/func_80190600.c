typedef signed short s16;
typedef unsigned char u8;

typedef struct {
    char pad_00[0x28];
    s16 field_28[4];
    char pad_30[8];
    u8 field_38[4];
    u8 field_3C[4];
    u8 field_40[4];
    u8 field_44;
} Ovl178SlotsState;

typedef struct {
    char pad_00[1];
    u8 status;
} Ovl178SlotsInput;

void func_80190600(void *arg0, Ovl178SlotsInput *input, Ovl178SlotsState *state) {
    register unsigned int i __asm__("t0");
    register unsigned int one __asm__("t1");
    register unsigned int eight __asm__("t3");
    register unsigned int done_status __asm__("t2");
    register u8 *field_28 __asm__("a3");
    register u8 *slot __asm__("a0");
    unsigned int value;

    i = 0;
    one = 1;
    eight = 8;
    done_status = 2;
    __asm__ volatile("" : : "r"(one), "r"(eight), "r"(done_status));
    field_28 = (u8 *)state;
    __asm__ volatile("" : "=r"(slot) : "0"(state));

    do {
        if (slot[0x40] != 0) {
            slot[0x40]--;
            if (slot[0x40] == 0) {
                slot[0x3C] = one;
            }
        }

        if (slot[0x3C] == one) {
            if (*(s16 *)(field_28 + 0x28) >= 0x11) {
                *(s16 *)(field_28 + 0x28) -= 0x10;
            }

            value = slot[0x38];
            slot[0x38] = value + 1;

            if (value == eight) {
                slot[0x3C] = 0;
                state->field_44--;
                if (state->field_44 == 0) {
                    input->status = done_status;
                }
            }
        }

        field_28 += 2;
        i++;
        slot++;
    } while (i < 4);
}
