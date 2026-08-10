int RotAverageNclip3(
    void *v0,
    void *v1,
    void *v2,
    void *sxy0,
    void *sxy1,
    void *sxy2,
    void *p,
    int *otz,
    int *flag) {
    register int nclipValue asm("$2");
    int flagValue;
    int otzValue;

    asm volatile("lwc2 $0,0(%0)" : : "r"(v0));
    asm volatile("lwc2 $1,4(%0)" : : "r"(v0));
    asm volatile("lwc2 $2,0(%0)" : : "r"(v1));
    asm volatile("lwc2 $3,4(%0)" : : "r"(v1));
    asm volatile("lwc2 $4,0(%0)" : : "r"(v2));
    asm volatile("lwc2 $5,4(%0)" : : "r"(v2));
    asm volatile("nop");
    asm volatile(".word 0x4A280030");
    asm volatile("cfc2 %0,$31" : "=r"(flagValue));
    *flag = flagValue;
    asm volatile(".word 0x4B400006");
    asm volatile("mfc2 %0,$24" : "=r"(nclipValue));
    if (nclipValue > 0) {
        asm volatile("swc2 $12,0(%0)" : : "r"(sxy0));
        asm volatile("swc2 $13,0(%0)" : : "r"(sxy1));
        asm volatile("swc2 $14,0(%0)" : : "r"(sxy2));
        asm volatile("swc2 $8,0(%0)" : : "r"(p));
        asm volatile("nop");
        asm volatile(".word 0x4B58002D");
        asm volatile("mfc2 %0,$7" : "=r"(otzValue));
        *otz = otzValue;
    }
    return nclipValue;
}
