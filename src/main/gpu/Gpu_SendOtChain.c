/* CC1_FLAGS: -g3 */
/* MASPSX_FLAGS: --stack-return-delay */

typedef int s32;

extern volatile s32 *D_80095864;
extern volatile s32 *D_8009586C;
extern volatile s32 *g_GpuDmaControlRegPtr;

s32 Gpu_DmaTimeoutCheck(void);

s32 Gpu_SendOtChain(s32 arg0, s32 arg1) {
    register s32 count asm("$16");
    register volatile s32 *status asm("$5");
    s32 offset;
    s32 temp_a0;

    count = arg1;
    status = g_GpuDmaControlRegPtr;
    *status |= 0x08000000;
    *D_8009586C = 0;
    offset = (count * 4) - 4;
    temp_a0 = arg0 + offset;
    *D_80095864 = temp_a0;

    asm volatile(
        ".set push\n"
        ".set noreorder\n"
        "lui $2, %%hi(D_80095868)\n"
        "lw $2, %%lo(D_80095868)($2)\n"
        "lui $3, 0x1100\n"
        "sw $16, 0($2)\n"
        "lui $2, %%hi(D_8009586C)\n"
        "lw $2, %%lo(D_8009586C)($2)\n"
        "ori $3, $3, 0x2\n"
        ".word 0x0C000000\n"
        ".reloc .-4,R_MIPS_26,Gpu_ResetDmaWaitTimer\n"
        "sw $3, 0($2)\n"
        ".set pop\n"
        :
        :
        : "$2", "$3", "$31", "memory");

    if (*D_8009586C & 0x01000000) {
        do {
            if (Gpu_DmaTimeoutCheck() != 0) {
                return -1;
            }
        } while (*D_8009586C & 0x01000000);
    }

    return count;
}
