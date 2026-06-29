extern unsigned int *g_GpuGp0Ptr;
extern unsigned int *g_GpuGp1Ptr;

int Gpu_WriteDataWords(unsigned int *src, int count) {
    int remaining;

    remaining = count - 1;
    *g_GpuGp1Ptr = 0x4000000;
    if (count != 0) {
        do {
            *g_GpuGp0Ptr = *src++;
            remaining--;
        } while (remaining != -1);
    }

    return 0;
}
