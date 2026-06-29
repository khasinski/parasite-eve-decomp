int Stat_BinarySearch(int value, int *table) {
    int step;
    int index;
    int result;

    index = 0x40;
    step = 0x20;

    while (step != 0) {
        if (value >= table[index]) {
            index += step;
        } else if (value < table[index - 1]) {
            index -= step;
        } else {
            step = 0;
        }
        step >>= 1;
    }

    index--;
    result = 0x62;
    if (index < 0x63) {
        result = index;
    }

    return result;
}
