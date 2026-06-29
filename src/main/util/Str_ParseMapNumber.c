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
