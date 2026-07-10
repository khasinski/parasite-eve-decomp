typedef unsigned int u32;

extern int D_8009B46C[];
extern int D_8009B424;
extern int D_8009B418;
extern int D_8009B434;
extern int D_8009B384;
extern char D_8009C4C0[];

int _SpuIsInAllocateArea_(int arg0);
int _spu_t(int arg0, ...);
int WaitEvent(int arg0);

int SPU_StartDmaRead(unsigned int index) {
    register unsigned int index_reg asm("$16");
    register int *area_ptr asm("$17");
    register int addr asm("$18");
    register int repeat asm("$19");
    register int saved_mode asm("$20");
    register int restore_mode asm("$21");
    volatile int saved_transfer;
    int block;
    int size;
    int alloc_size;

    index_reg = index;
    saved_transfer = 0;
    restore_mode = 0;

    if (index_reg >= 10) {
        return -1;
    }

    area_ptr = &D_8009B46C[index_reg];
    alloc_size = *area_ptr;
    if (_SpuIsInAllocateArea_(alloc_size) != 0) {
        return -1;
    }

    if (index_reg == 0) {
        size = 0x10 << D_8009B424;
        addr = 0xFFF0 << D_8009B424;
    } else {
        alloc_size = *area_ptr;
        addr = alloc_size << D_8009B424;
        size = (0x10000 - alloc_size) << D_8009B424;
    }

    saved_mode = D_8009B418;
    if (saved_mode == 1) {
        D_8009B418 = 0;
        restore_mode = 1;
    }

    repeat = 1;
    if (D_8009B434 != 0) {
        saved_transfer = D_8009B434;
        D_8009B434 = 0;
    }

    do {
        if (size < 0x401U) {
            block = size;
            repeat = 0;
        } else {
            block = 0x400;
        }

        _spu_t(2, addr);
        _spu_t(1);
        _spu_t(3, D_8009C4C0, block);
        size -= 0x400;
        addr += 0x400;
        WaitEvent(D_8009B384);
    } while (repeat != 0);

    if (restore_mode != 0) {
        D_8009B418 = saved_mode;
    }

    if (saved_transfer != 0) {
        D_8009B434 = saved_transfer;
    }

    return 0;
}
