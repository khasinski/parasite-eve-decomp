
/* CC1_FLAGS: -O1 */
/* MASPSX_FLAGS: --store-return-delay */

extern unsigned int *g_GpuGp1Ptr;
extern unsigned char D_800A3348[];

void Gpu_WriteControlAndMirror(unsigned int value) {
    unsigned int index;

    *g_GpuGp1Ptr = value;
    index = value >> 24;
    D_800A3348[index] = value;
}
