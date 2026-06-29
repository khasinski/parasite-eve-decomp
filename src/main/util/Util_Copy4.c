void Util_Copy4(unsigned char *dst, unsigned char *src) {
    int i;

    if (src) {
        if (dst) {
            for (i = 0; i < 4; i++) {
                *dst++ = *src++;
            }
        }
    } else if (dst) {
        *dst = 0;
    }
}
