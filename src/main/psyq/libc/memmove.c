void *memmove(void *dst, const void *src, int len) {
    unsigned char *out = dst;
    const unsigned char *in = src;
    register int count asm("$6") = len;
    register int test asm("$2");
    void *ret;
    if (out >= in) {
        test = count;
        count--;
        if (test > 0) {
            do {
                out[count] = in[count];
            } while (count-- > 0);
        }
        goto done;
    } else {
        test = count;
        count--;
        if (test > 0) {
            do {
                *out++ = *in++;
            } while (count-- > 0);
        }
    }

done:
    ret = out;
    asm volatile("" : "=r"(ret) : "0"(ret));
    return ret;
}
