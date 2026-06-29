typedef unsigned char u8;

void Util_AppendFFTerminatedBytes(u8 *dst, u8 *src)
{
    int scan;
    u8 first;

    scan = *dst++;
    if (scan != 0xFF) {
        int term;
        int next;

        term = 0xFF;
        do {
            next = *dst++;
        } while (next != term);
    }

    dst--;

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
