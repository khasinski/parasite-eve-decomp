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
