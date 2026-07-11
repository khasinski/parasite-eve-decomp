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

    asm volatile("lwc2 $0,0(%0)" : : "r"(v0));
    asm volatile("lwc2 $1,4(%0)" : : "r"(v0));
    asm volatile("lwc2 $2,0(%0)" : : "r"(v1));
    asm volatile("lwc2 $3,4(%0)" : : "r"(v1));
    asm volatile("lwc2 $4,0(%0)" : : "r"(v2));
    asm volatile("lwc2 $5,4(%0)" : : "r"(v2));
    asm volatile("nop");
    asm volatile(".word 0x4A280030");
    asm volatile("swc2 $12,0(%0)" : : "r"(sxy0));
    asm volatile("swc2 $13,0(%0)" : : "r"(sxy1));
    asm volatile("swc2 $14,0(%0)" : : "r"(sxy2));
    asm volatile("cfc2 %0,$31" : "=r"(firstFlag));
    asm volatile("lwc2 $0,0(%0)" : : "r"(v3));
    asm volatile("lwc2 $1,4(%0)" : : "r"(v3));
    asm volatile("nop");
    asm volatile(".word 0x4A180001");
    asm volatile("swc2 $14,0(%0)" : : "r"(sxy3));
    asm volatile("swc2 $8,0(%0)" : : "r"(p));
    asm volatile("cfc2 %0,$31" : "=r"(secondFlag));
    asm volatile("mfc2 %0,$19" : "=r"(result));
    secondFlag |= firstFlag;
    *flag = secondFlag;
    return result >> 2;
}
