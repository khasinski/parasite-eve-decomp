typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;

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

int func_800CC0E0(void *arg0, void *arg1, u8 *anim) {
    register u16 *base_s5 asm("$21");
    register int neg_s6 asm("$22");
    int i;
    int count;
    int angle;
    u8 *pos_entry;
    u8 *rot_entry;
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
        rot_entry = anim;
        do {
            *(u16 *)(rot_entry + 0x26) = base_s5[0];
            *(u16 *)(rot_entry + 0x28) = base_s5[1];
            *(u16 *)(rot_entry + 0x2A) = base_s5[2];

            if ((s8)anim[2] == 0x10) {
                angle = i << 8;
            } else {
                angle = i << 9;
            }

            *(u16 *)(rot_entry + 0xA6) = rsin(angle);
            *(u16 *)(rot_entry + 0xA8) = neg_s6;
            *(u16 *)(rot_entry + 0xAA) = rcos(angle);
            *(u16 *)(pos_entry + 0x6) = 0x258;

            pos_entry += 2;
            count = (s8)anim[2];
            asm volatile("" : "=r"(count) : "0"(count));
            i++;
            rot_entry += 8;
        } while (i < count);
    }
}
