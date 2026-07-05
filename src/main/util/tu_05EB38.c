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
