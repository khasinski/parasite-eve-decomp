typedef unsigned int u32;

extern void *g_CdRingBufPtr;

void init_ring_status(int start, u32 count) {
    u32 i;

    i = 0;
    if (count != 0) {
        do {
            *(int *)((char *)g_CdRingBufPtr + ((i + start) << 5)) = 0;
            i++;
        } while (i < count);
    }
}
