/* CC1_PROFILE: build0001 */

typedef unsigned char u8;

extern u8 g_Base32CharTable[];

int Str_EncodeBase32(char *out, unsigned int value) {
    int i;
    int last;
    char *dst;

    i = 0;
    last = 5;
    dst = out;
    while (i < 6) {
        int shift = ((last - i) * 5) + 2;
        u8 c = g_Base32CharTable[(value >> shift) & 0x1F];

        *dst = c;
        if ((unsigned int)(c - 0x61) < 0x1A) {
            *dst = c - 0x20;
        }
        i++;
        dst++;
    }

    out[6] = 0;
    return 0;
}
