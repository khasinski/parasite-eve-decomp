extern signed char g_Base32CharTable[];

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
