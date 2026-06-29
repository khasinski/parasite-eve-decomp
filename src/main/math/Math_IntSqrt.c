int Math_IntSqrt(int value)
{
    int result = 0;
    int shift = 30;

    do {
        int step = ((result << 2) + 1) << shift;

        result <<= 1;
        if (value >= step) {
            value -= step;
            result |= 1;
        }

        shift -= 2;
    } while (shift >= 0);

    return result;
}
