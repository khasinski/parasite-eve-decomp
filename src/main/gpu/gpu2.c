extern unsigned char g_GpuControlRegMirror[];

extern unsigned int *g_GpuGp0Ptr;
extern unsigned int *g_GpuGp1Ptr;

int Gpu_GetControlMirrorByte(int arg0) {
    int ret;

    asm volatile(
        "lui %0,%%hi(g_GpuControlRegMirror)\n"
        "addu %0,%0,%1\n"
        "lbu %0,%%lo(g_GpuControlRegMirror)(%0)"
        : "=r"(ret)
        : "r"(arg0)
    );

    return ret;
}

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
