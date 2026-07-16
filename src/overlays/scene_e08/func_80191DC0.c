typedef short s16;
typedef int s32;
typedef unsigned int u32;

s32 func_80071A54(void);

void func_80191DC0(void *arg0, void *arg1, char *arg2) {
    register u32 i __asm__("s2");
    register s32 magic __asm__("s4");
    register char *p2 __asm__("s0");
    register char *p8 __asm__("s1");
    register char *p4 __asm__("s3");
    s32 value;

    i = 0;
    magic = 0x66666667;
    p2 = arg2;
    p8 = p2;
    p4 = p2;


    *(s16 *)(p2 + 0x58) = 0;
    *(s16 *)(p2 + 0x5A) = 0;

    do {
        *(s32 *)(p4 + 0x28) = i << 11;
        *(s16 *)(p2 + 0x38) = 0;
        *(s16 *)(p8 + 0x8) = 0;
        *(s16 *)(p8 + 0xA) = 0;
        *(s16 *)(p8 + 0xC) = 0;
        value = func_80071A54();
        __asm__ volatile("mult %0,%1" : : "r"(value), "r"(magic));
        p8 += 8;
        p4 += 4;
        i++;
        __asm__ volatile(
            "sra $3,%1,31\n"
            "mfhi $5\n"
            "sra $4,$5,2\n"
            "subu $4,$4,$3\n"
            "sll $3,$4,2\n"
            "addu $3,$3,$4\n"
            "sll $3,$3,1\n"
            "subu %0,%1,$3\n"
            "addiu %0,%0,10"
            : "=r"(value)
            : "r"(value)
            : "$3", "$4", "$5");
        *(s16 *)(p2 + 0x48) = value;
        p2 += 2;
    } while (i < 4);
}
