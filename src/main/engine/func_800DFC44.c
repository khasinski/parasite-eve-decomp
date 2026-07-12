int func_800DFC44(int value) {
    int result = 0;
    int shift = 0x1E;

    do {
        int threshold = ((result << 2) + 1) << shift;

        result <<= 1;
        if (value >= threshold) {
            value -= threshold;
            result |= 1;
        }
        shift -= 2;
    } while (shift >= 0);

    return result;
}
