typedef unsigned char u8;

void Util_CopyFFTerminatedBytes(u8 *dst, u8 *src)
{
    u8 first;

    first = *src++;
    *dst++ = first;
    if (first != 0xFF) {
        int term;
        int next;

        term = 0xFF;
        do {
            next = *src++;
            *dst++ = next;
        } while (next != term);
    }
}
