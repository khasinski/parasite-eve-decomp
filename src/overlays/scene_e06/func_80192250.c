extern char *D_8009D254;
extern int D_800966EC[];

int FieldEng_VecToAngle(int *vec, int *ref);
void D_8019235C(void);

void func_80192250(char *ent) {
    register char *o asm("$16");
    register char *actor asm("$4");
    register void (*callback)(void) asm("$2");
    register int minusOne asm("$17");
    int *signal;
    int *entry;
    int angle;
    int lo;
    int hi;
    int savedHi;
    int cur;
    register int mask asm("$2");
    register int flags asm("$3");

    o = ent;
    actor = D_8009D254;
    if (*(unsigned char *)(actor + 0xE) < 4) {
        mask = 0xFFF3FFFF;
        flags = *(int *)(actor + 0x98);
        flags &= mask;
        *(int *)(actor + 0x98) = flags;

        signal = *(int **)(o + 0x10);
        callback = D_8019235C;
        *(void (**)(void))(o + 0xC) = callback;
        if (signal != 0) {
            *signal = 2;
        }

        cur = *(short *)(o + 0x44);
        minusOne = -1;
        if (cur == minusOne) {
            *(short *)(o + 0x44) = FieldEng_VecToAngle(
                (int *)(*(char **)(o + 8) + 0x28),
                (int *)(D_8009D254 + 0x28));
        }

        if (*(short *)(o + 0x48) == minusOne) {
            *(short *)(o + 0x48) = *(unsigned short *)(o + 0x44) + 0x800;
        }

        angle = *(unsigned short *)(o + 0x44) & 0xFFF;
        entry = (int *)((char *)D_800966EC + (angle << 2));
        hi = *(short *)((char *)entry + 2);
        *(volatile short *)(o + 0x1E) = 0;
        *(volatile short *)(o + 0x1C) = hi;
        lo = *entry;
        savedHi = *(unsigned short *)(o + 0x1C);
        *(volatile short *)(o + 0x22) = 0;
        *(volatile short *)(o + 0x26) = 0;
        *(volatile short *)(o + 0x2A) = 0;
        *(volatile short *)(o + 0x20) = lo;
        *(volatile short *)(o + 0x24) = 0x1000;
        *(volatile short *)(o + 0x2C) = savedHi;
        *(volatile short *)(o + 0x28) = -*(unsigned short *)(o + 0x20);
    }
}
