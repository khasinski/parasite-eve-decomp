#include "common.h"
/* CC1_FLAGS: -fno-strength-reduce */
int *func_800C2B10(int index);
int rsin(int angle);
int rcos(int angle);

extern u8 D_800E2280;
extern u8 D_800E2281;
extern u8 D_800E2282;
extern u16 D_800E2290;
extern u16 D_800E2292;
extern u16 D_800E2294;
extern char *D_8009D254;

void FieldAnim_InitRadialEntries(void *arg0, void *arg1, u8 *anim) {
    u16 *base_s5;
    volatile int stack_pad;
    int neg_s6;
    int i;
    int angle;
    u8 *pos_entry;
    u16 *rot_entry;
    char *entity;
    char *core;
    char *mode_data;

    D_800E2280 = *func_800C2B10(3);
    D_800E2281 = *func_800C2B10(4);
    D_800E2282 = *func_800C2B10(5);

    anim[1] = 0x7F;
    entity = D_8009D254;
    core = *(char **)entity;
    mode_data = *(char **)(core + 0x68);
    if (*(short *)(mode_data + 0x6) == 3) {
        anim[2] = 8;
    } else {
        anim[2] = 0x10;
    }

    if ((s8)anim[2] > 0) {
        i = 0;
        base_s5 = &D_800E2290;
        neg_s6 = -0x1400;
        pos_entry = anim;
        rot_entry = (u16 *)anim;
        do {
            rot_entry[0x13] = base_s5[0];
            rot_entry[0x14] = base_s5[1];
            rot_entry[0x15] = base_s5[2];

            if ((s8)anim[2] == 0x10) {
                angle = i << 8;
            } else {
                angle = i << 9;
            }

            rot_entry[0x53] = rsin(angle);
            rot_entry[0x54] = neg_s6;
            rot_entry[0x55] = rcos(angle);
            *(u16 *)(pos_entry + 0x6) = 0x258;

            pos_entry += 2;
            rot_entry += 4;
        } while (++i < (s8)anim[2]);
    }
}
