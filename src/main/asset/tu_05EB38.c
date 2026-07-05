extern signed char g_Base32CharTable[];

typedef unsigned int u32;
typedef unsigned char u8;
typedef int s32;

int Str_DecodeBase32(unsigned int a0) {
    int total = 0;
    int i;

    for (i = 1; i < 5; i++) {
        int factor = 1;
        int j;

        for (j = 0; j < 4 - i; j++) {
            factor *= 10;
        }

        total += (g_Base32CharTable[(a0 >> ((5 * (5 - i)) + 2)) & 0x1F] - 0x30) * factor;
    }

    return total;
}

int Str_ParseBase32Id(signed char *arg0) {
    unsigned int result = 0;
    int index = 0;

    while (index < 6) {
        signed char c = *arg0;
        int value = ((unsigned int)(c - '0') < 10U)
            ? (c - '0')
            : (c - 'W') - (c >= 'k') - (c >= 'r') - (c >= 'y');

        result |= (unsigned int)value << ((5 - index) * 5 + 2);
        index++;
        arg0++;
    }

    return result;
}

int Str_ParseMapNumber(signed char *arg0) {
    int hundreds;
    int tens;
    int result;

    hundreds = arg0[2] - 0x30;
    result = hundreds * 3;
    result <<= 3;
    result += hundreds;
    result <<= 2;

    tens = arg0[3] - 0x30;
    result += tens * 10;
    result -= 0x30;

    return result + arg0[4];
}

int Asset_FindTable08ByU32Key(void *arg0, s32 arg1) {
    u32 mask;
    register u8 *table asm("$2");
    u32 descriptor;
    register u8 *record asm("$6");
    register s32 none asm("$7");
    register u32 i asm("$8");
    u32 data_mask;
    char frame[1];

    asm volatile("" : : "r"(frame));

    mask = 0x3FFFFF;
    descriptor = *(u32 *)((u8 *)arg0 + *(u32 *)((u8 *)arg0 + 4) + 8);
    i = 0;
    none = 0;
    table = (u8 *)arg0 + (descriptor & mask);
    descriptor >>= 22;

    if (descriptor != 0) {
        data_mask = 0xFFFFFF;
        record = table;
        do {
            if (*(s32 *)(record + 8) == arg1) {
                return (s32)((u8 *)arg0 + (*(u32 *)(record + 4) & data_mask));
            }
            i++;
            if (i < descriptor) {
                record += 0xC;
                continue;
            }
            break;
        } while (1);
    }

    return none;
}
