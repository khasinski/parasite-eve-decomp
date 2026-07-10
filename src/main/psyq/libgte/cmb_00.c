int RotTransPers4(
    void *v0,
    void *v1,
    void *v2,
    void *v3,
    void *sxy0,
    void *sxy1,
    void *sxy2,
    void *sxy3,
    void *p,
    int *flag) {
    register int firstFlag asm("$3");
    register int secondFlag asm("$8");
    register int result asm("$2");
    register void *sxy0Ptr asm("$8");
    register void *sxy1Ptr asm("$9");
    register void *sxy2Ptr asm("$10");
    register void *sxy3Ptr asm("$8");
    register void *pPtr asm("$9");
    register void *flagPtr asm("$10");

    asm volatile("lwc2 $0,0(%0)" : : "r"(v0));
    asm volatile("lwc2 $1,4(%0)" : : "r"(v0));
    asm volatile("lwc2 $2,0(%0)" : : "r"(v1));
    asm volatile("lwc2 $3,4(%0)" : : "r"(v1));
    asm volatile("lwc2 $4,0(%0)" : : "r"(v2));
    asm volatile("lwc2 $5,4(%0)" : : "r"(v2));
    asm volatile("nop");
    asm volatile(".word 0x4A280030");
    asm volatile("lw %0,0x10($sp)" : "=r"(sxy0Ptr));
    asm volatile("lw %0,0x14($sp)" : "=r"(sxy1Ptr));
    asm volatile("lw %0,0x18($sp)" : "=r"(sxy2Ptr));
    asm volatile("swc2 $12,0(%0)" : : "r"(sxy0Ptr));
    asm volatile("swc2 $13,0(%0)" : : "r"(sxy1Ptr));
    asm volatile("swc2 $14,0(%0)" : : "r"(sxy2Ptr));
    asm volatile("cfc2 %0,$31" : "=r"(firstFlag));
    asm volatile("lwc2 $0,0(%0)" : : "r"(v3));
    asm volatile("lwc2 $1,4(%0)" : : "r"(v3));
    asm volatile("nop");
    asm volatile(".word 0x4A180001");
    asm volatile("lw %0,0x1C($sp)" : "=r"(sxy3Ptr));
    asm volatile("lw %0,0x20($sp)" : "=r"(pPtr));
    asm volatile("lw %0,0x24($sp)" : "=r"(flagPtr));
    asm volatile("swc2 $14,0(%0)" : : "r"(sxy3Ptr));
    asm volatile("swc2 $8,0(%0)" : : "r"(pPtr));
    asm volatile("cfc2 %0,$31" : "=r"(secondFlag));
    asm volatile("mfc2 %0,$19" : "=r"(result));
    secondFlag |= firstFlag;
    asm volatile("sw %0,0(%1)" : : "r"(secondFlag), "r"(flagPtr));
    return result >> 2;
}

asm(".word 0x00000000");
asm(".word 0x00000000");
