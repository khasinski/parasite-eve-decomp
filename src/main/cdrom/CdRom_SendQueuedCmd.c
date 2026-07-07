/* MASPSX_FLAGS: -G8 --stack-return-delay */

typedef unsigned int u32;
typedef unsigned char u8;

extern u32 D_800A3520[];
extern u32 D_800A3530[];

int CdRom_SendReadyCommand(int arg);
void Util_Copy8(void *dst, void *src);

int CdRom_SendQueuedCmd(void *dst) {
    register void *dst_reg asm("$16");
    register void *dst_arg asm("$4");
    register u32 *slot asm("$16");
    register int temp asm("$2");
    int mode;

    dst_reg = dst;
    asm volatile("" : "=r"(dst_reg) : "0"(dst_reg));
    CdRom_SendReadyCommand(0);

    if (D_800A3530[0] == 1) {
        mode = 4;
    } else {
        temp = D_800A3520[0] ^ 1;
        mode = (temp < 1U);
    }

    if (mode == 4) {
        dst_arg = dst_reg;
        asm volatile("" : "=r"(dst_arg) : "0"(dst_arg));
        slot = D_800A3530;
    } else if (mode == 1) {
        dst_arg = dst_reg;
        asm volatile("" : "=r"(dst_arg) : "0"(dst_arg));
        slot = D_800A3520;
    } else {
        return 0;
    }

    slot[0] = 0;
    Util_Copy8(dst_arg, ((u8 *)slot) + 5);
    return ((u8 *)slot)[4];
}
