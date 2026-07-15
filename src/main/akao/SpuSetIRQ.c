typedef unsigned short u16;

extern void *D_8009B3FC;
extern char D_8001208C[];
extern char D_8001209C[];
extern char D_800120AC[];

int printf(char *fmt, char *arg);

#define SPU_REG16(off) (*(u16 *)((char *)D_8009B3FC + (off)))

int SpuSetIRQ(int mode) {
    int count;

    if ((mode == 0) || (mode == 3)) {
        SPU_REG16(0x1AA) &= 0xFFBF;
        if ((SPU_REG16(0x1AA) & 0x40) != 0) {
            count = 0;
            count++;
            while (count < 0xF01) {
                if ((SPU_REG16(0x1AA) & 0x40) == 0) {
                    count--;
                    break;
                }
                count++;
            }
            if (count >= 0xF01) {
                printf(D_8001208C, D_8001209C);
                return -1;
            }
        }
    }

    if ((mode == 1) || (mode == 3)) {
        SPU_REG16(0x1AA) |= 0x40;
        if ((SPU_REG16(0x1AA) & 0x40) == 0) {
            count = 0;
            count++;
            while (count < 0xF01) {
                if ((SPU_REG16(0x1AA) & 0x40) != 0) {
                    count--;
                    break;
                }
                count++;
            }
            if (count >= 0xF01) {
                printf(D_8001208C, D_800120AC);
                return -1;
            }
        }
    }

    return mode;
}
