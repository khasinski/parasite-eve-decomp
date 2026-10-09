/* CC1_FLAGS: -fno-strength-reduce */
#include "common.h"
#include "pe1/field_textured_chain_node.h"

int rand(void);
int func_800C6B20(void *arg0);


int func_800C5EB0(char *data, void *unused, int *hit) {
    GteShortVector local[5];
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
    if ((u32)idx < count) {
        entries[idx * 0x44] = 2;
    }

    countMinus = *(s16 *)(data_s3 + 0x4);
    countMinus -= 1;
    i = 0;
    if (countMinus != 0) {
        register FieldTexturedChainNode *entry asm("$17");
        char *body;
        entry = (FieldTexturedChainNode *)entries;
        body = entries + 4;
        do {
            int rnd = rand();

            body[-1] = rnd % 4;

            if (entry->visible == 2) {
                local[0] = *(GteShortVector *)(body + 16);
                local[1] = *(GteShortVector *)(body + 24);
                local[2] = *(GteShortVector *)(body + 84);
                local[3] = *(GteShortVector *)(body + 92);

                local[0].y = 0;
                local[1].y = 0;
                local[2].y = 0;
                local[3].y = 0;

                if (func_800C6B20(local) == 1) {
                    *hit_s4 = 1;
                }

                if (*(u16 *)body >= 0xD) {
                    *(u16 *)body -= 0xC;
                } else {
                    *(u16 *)body = 0;
                    entry->visible = 0;
                }
            }
            i++;
            body += 0x44;
            entry++;
        } while (i < *(s16 *)(data_s3 + 0x4) - 1);
    }

    timer = *(volatile u16 *)(data_s3 + 0xE);
    i = ((s16)timer == 0x64);
    *(u16 *)(data_s3 + 0xE) = timer + 1;

    return i;
}
