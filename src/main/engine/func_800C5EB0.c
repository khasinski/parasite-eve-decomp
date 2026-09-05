/* CC1_FLAGS: -fno-strength-reduce */
#include "common.h"
typedef struct {
    s16 values[4];
} HalfwordBlock8;

int rand(void);
int func_800C6B20(void *arg0);

typedef struct {
    u8 state;
    u8 pad1;
    u8 pad2;
    u8 random;
    u16 timer;
} RingEntry;

int func_800C5EB0(char *data, void *unused, int *hit) {
    HalfwordBlock8 local[5];
    char *data_s3 = data;
    char *entries;
    int *hit_s4;
    u32 i;
    int idx;
    int count;
    int countMinus;
    u16 timer;

    entries = *(char **)data_s3;
    hit_s4 = hit;
    *hit_s4 = 0;

    idx = *(s16 *)(data_s3 + 0xE);
    count = *(s16 *)(data_s3 + 0x4);
    if ((u32)idx < (u32)count) {
        entries[idx * 0x44] = 2;
    }

    countMinus = *(s16 *)(data_s3 + 0x4);
    countMinus -= 1;
    i = 0;
    if (countMinus != 0) {
        register char *entry asm("$17");
        char *body;
        entry = entries;
        body = entries + 4;
        do {
            int rnd = rand();

            body[-1] = rnd % 4;

            if (entry[0] == 2) {
                local[0] = *(HalfwordBlock8 *)(body + 16);
                local[1] = *(HalfwordBlock8 *)(body + 24);
                local[2] = *(HalfwordBlock8 *)(body + 84);
                local[3] = *(HalfwordBlock8 *)(body + 92);

                local[0].values[1] = 0;
                local[1].values[1] = 0;
                local[2].values[1] = 0;
                local[3].values[1] = 0;

                if (func_800C6B20(local) == 1) {
                    *hit_s4 = 1;
                }

                if (*(u16 *)body >= 0xD) {
                    *(u16 *)body -= 0xC;
                } else {
                    *(u16 *)body = 0;
                    entry[0] = 0;
                }
            }
            i++;
            body += 0x44;
            entry += 0x44;
        } while (i < *(s16 *)(data_s3 + 0x4) - 1);
    }

    timer = *(volatile u16 *)(data_s3 + 0xE);
    i = ((s16)timer == 0x64);
    *(u16 *)(data_s3 + 0xE) = timer + 1;

    return i;
}
