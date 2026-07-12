typedef unsigned char u8;
typedef unsigned short u16;
typedef signed short s16;

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
    char local[0x20];
    char *entries = *(char **)data;
    int i;

    *hit = 0;

    if (*(s16 *)(data + 0xE) < *(s16 *)(data + 0x4)) {
        entries[(*(s16 *)(data + 0xE) * 0x44)] = 2;
    }

    for (i = 0; i < *(s16 *)(data + 0x4) - 1; i++) {
        char *entry = entries + i * 0x44;
        char *body = entry + 4;
        int rnd = rand();

        body[-1] = rnd % 4;

        if (entry[0] == 2) {
            *(long long *)(local + 0x00) = *(long long *)(body + 0x10);
            *(long long *)(local + 0x08) = *(long long *)(body + 0x18);
            *(long long *)(local + 0x10) = *(long long *)(body + 0x54);
            *(long long *)(local + 0x18) = *(long long *)(body + 0x5C);

            *(s16 *)(local + 0x2) = 0;
            *(s16 *)(local + 0xA) = 0;
            *(s16 *)(local + 0x12) = 0;
            *(s16 *)(local + 0x1A) = 0;

            if (func_800C6B20(local) == 1) {
                *hit = 1;
            }

            if (*(u16 *)body >= 0xD) {
                *(u16 *)body -= 0xC;
            } else {
                *(u16 *)body = 0;
                entry[0] = 0;
            }
        }
    }

    i = (*(s16 *)(data + 0xE) == 0x64);
    *(u16 *)(data + 0xE) += 1;

    return i;
}
