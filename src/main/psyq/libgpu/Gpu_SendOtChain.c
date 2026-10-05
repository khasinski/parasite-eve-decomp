/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */
extern volatile unsigned int *D_80095870;
extern volatile unsigned int *D_8009586C;
extern volatile unsigned int *D_80095864;
extern volatile unsigned int *D_80095868;
void Gpu_ResetDmaWaitTimer(void);
int Gpu_DmaTimeoutCheck(void);

/* Initialize a reverse ordering table through the OTC DMA channel. */
int Gpu_SendOtChain(unsigned int *table, int count)
{
    unsigned int status;
    register unsigned int firstMask asm("$3");
    *D_80095870 |= 0x08000000;
    *D_8009586C = 0;
    *D_80095864 = (unsigned int)((char *)table + (count * 4 - 4));
    *D_80095868 = count;
    /* This store occupies the following call's delay slot in retail. */
    *(unsigned int *)D_8009586C = 0x11000002;
    Gpu_ResetDmaWaitTimer();
    status = *D_8009586C;
    /* Keep the first status test separate from the loop's invariant mask. */
    asm volatile("" : "+r"(status), "+r"(count));
    firstMask = 0x01000000;
    asm volatile("" : "+r"(firstMask) : "r"(status));
    if (status & firstMask) {
        asm volatile("" : : "r"(count));
        do {
            if (Gpu_DmaTimeoutCheck()) return -1;
        } while (*D_8009586C & 0x01000000);
    }
    return count;
}
