void GPU_memset(unsigned char *dst, int value, int count)
{
    int remaining;

    remaining = count - 1;
    if (count != 0) {
        do {
            *dst++ = value;
            remaining--;
        } while (remaining != -1);
    }
}
