
/* CC1_FLAGS: -O1 */

extern unsigned int *g_GpuGp1Ptr;
register unsigned char *g_GpuControlMirror asm("$1");

void Gpu_WriteControlAndMirror(unsigned int value) {
    unsigned int index;

    *g_GpuGp1Ptr = value;
    index = value >> 24;
    g_GpuControlMirror = (unsigned char *)0x800A0000;
    g_GpuControlMirror += index;
    g_GpuControlMirror[0x3348] = value;
}
