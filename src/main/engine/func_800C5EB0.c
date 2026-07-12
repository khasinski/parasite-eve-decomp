typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef signed short s16;

#define COPY_BODY_TO_LOCAL(body) \
    asm volatile( \
        "lwl $2,19(%0)\n\t" \
        "lwr $2,16(%0)\n\t" \
        "lwl $3,23(%0)\n\t" \
        "lwr $3,20(%0)\n\t" \
        "swl $2,19($sp)\n\t" \
        "swr $2,16($sp)\n\t" \
        "swl $3,23($sp)\n\t" \
        "swr $3,20($sp)\n\t" \
        "lwl $2,27(%0)\n\t" \
        "lwr $2,24(%0)\n\t" \
        "lwl $3,31(%0)\n\t" \
        "lwr $3,28(%0)\n\t" \
        "swl $2,27($sp)\n\t" \
        "swr $2,24($sp)\n\t" \
        "swl $3,31($sp)\n\t" \
        "swr $3,28($sp)\n\t" \
        "lwl $2,87(%0)\n\t" \
        "lwr $2,84(%0)\n\t" \
        "lwl $3,91(%0)\n\t" \
        "lwr $3,88(%0)\n\t" \
        "swl $2,35($sp)\n\t" \
        "swr $2,32($sp)\n\t" \
        "swl $3,39($sp)\n\t" \
        "swr $3,36($sp)\n\t" \
        "lwl $2,95(%0)\n\t" \
        "lwr $2,92(%0)\n\t" \
        "lwl $3,99(%0)\n\t" \
        "lwr $3,96(%0)\n\t" \
        "swl $2,43($sp)\n\t" \
        "swr $2,40($sp)\n\t" \
        "swl $3,47($sp)\n\t" \
        "swr $3,44($sp)" \
        : \
        : "r"(body) \
        : "$2", "$3", "memory")

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
    char local[0x28];
    register char *data_s3 asm("$19") = data;
    register char *entries asm("$17");
    register int *hit_s4 asm("$20");
    u32 i;
    int idx;
    int count;
    int countMinus;
    u16 timer;

    asm("" : "=r"(data_s3) : "0"(data_s3));
    entries = *(char **)data_s3;
    asm volatile("move %0,%1" : "=r"(hit_s4) : "r"(hit));
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
        register char *body asm("$16");

        entry = entries;
        body = entries + 4;
        do {
            int rnd = rand();

            body[-1] = rnd % 4;

            if (entry[0] == 2) {
                COPY_BODY_TO_LOCAL(body);

                *(s16 *)(local + 0x2) = 0;
                *(s16 *)(local + 0xA) = 0;
                *(s16 *)(local + 0x12) = 0;
                *(s16 *)(local + 0x1A) = 0;

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
