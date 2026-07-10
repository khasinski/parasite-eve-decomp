int RotTransPers3(
    void *v0,
    void *v1,
    void *v2,
    void *sxy0,
    void *sxy1,
    void *sxy2,
    void *p,
    int *flag) {
    register int flagValue asm("$3");
    register int result asm("$2");
    register void *sxy1Ptr asm("$8");
    register void *sxy2Ptr asm("$9");
    register void *pPtr asm("$10");
    register void *flagPtr asm("$11");

    asm volatile("lwc2 $0,0(%0)" : : "r"(v0));
    asm volatile("lwc2 $1,4(%0)" : : "r"(v0));
    asm volatile("lwc2 $2,0(%0)" : : "r"(v1));
    asm volatile("lwc2 $3,4(%0)" : : "r"(v1));
    asm volatile("lwc2 $4,0(%0)" : : "r"(v2));
    asm volatile("lwc2 $5,4(%0)" : : "r"(v2));
    asm volatile("nop");
    asm volatile(".word 0x4A280030");
    asm volatile("lw %0,0x10($sp)" : "=r"(sxy1Ptr));
    asm volatile("lw %0,0x14($sp)" : "=r"(sxy2Ptr));
    asm volatile("lw %0,0x18($sp)" : "=r"(pPtr));
    asm volatile("lw %0,0x1C($sp)" : "=r"(flagPtr));
    asm volatile("swc2 $12,0(%0)" : : "r"(sxy0));
    asm volatile("swc2 $13,0(%0)" : : "r"(sxy1Ptr));
    asm volatile("swc2 $14,0(%0)" : : "r"(sxy2Ptr));
    asm volatile("swc2 $8,0(%0)" : : "r"(pPtr));
    asm volatile("cfc2 %0,$31" : "=r"(flagValue));
    asm volatile("mfc2 %0,$19" : "=r"(result));
    asm volatile("sw %0,0(%1)" : : "r"(flagValue), "r"(flagPtr));
    return result >> 2;
}

asm(".word 0x00000000");
asm(".word 0x00000000");
asm(".word 0x00000000");
