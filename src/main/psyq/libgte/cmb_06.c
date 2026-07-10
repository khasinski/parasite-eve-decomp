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
    register int flagValue asm("$9");
    register void *sxy1Ptr asm("$8");
    register void *sxy2Ptr asm("$9");
    register void *pPtr asm("$10");
    register void *otzPtr asm("$9");
    register void *flagPtr asm("$8");
    register int otzValue asm("$8");

    asm volatile("lwc2 $0,0(%0)" : : "r"(v0));
    asm volatile("lwc2 $1,4(%0)" : : "r"(v0));
    asm volatile("lwc2 $2,0(%0)" : : "r"(v1));
    asm volatile("lwc2 $3,4(%0)" : : "r"(v1));
    asm volatile("lwc2 $4,0(%0)" : : "r"(v2));
    asm volatile("lwc2 $5,4(%0)" : : "r"(v2));
    asm volatile("nop");
    asm volatile(".word 0x4A280030");
    asm volatile("lw %0,0x20($sp)" : "=r"(flagPtr));
    asm volatile("cfc2 %0,$31" : "=r"(flagValue));
    asm volatile("nop");
    asm volatile("sw %0,0(%1)" : : "r"(flagValue), "r"(flagPtr));
    asm volatile(".word 0x4B400006");
    asm volatile("lw %0,0x10($sp)" : "=r"(sxy1Ptr));
    asm volatile("lw %0,0x14($sp)" : "=r"(sxy2Ptr));
    asm volatile("lw %0,0x18($sp)" : "=r"(pPtr));
    asm volatile("mfc2 %0,$24" : "=r"(nclipValue));
    asm volatile("nop");
    asm volatile("bgtz %0,1f" : : "r"(nclipValue));
    asm volatile("b 2f");
    asm volatile("nop");
    asm volatile("1:");
    asm volatile("swc2 $12,0(%0)" : : "r"(sxy0));
    asm volatile("swc2 $13,0(%0)" : : "r"(sxy1Ptr));
    asm volatile("swc2 $14,0(%0)" : : "r"(sxy2Ptr));
    asm volatile("swc2 $8,0(%0)" : : "r"(pPtr));
    asm volatile("nop");
    asm volatile(".word 0x4B58002D");
    asm volatile("lw %0,0x1C($sp)" : "=r"(otzPtr));
    asm volatile("mfc2 %0,$7" : "=r"(otzValue));
    asm volatile("nop");
    asm volatile("sw %0,0(%1)" : : "r"(otzValue), "r"(otzPtr));
    asm volatile("2:");
    return nclipValue;
}

asm(".word 0x00000000");
asm(".word 0x00000000");
