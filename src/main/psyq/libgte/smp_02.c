int RotTransPers(void *v, void *sxy, void *p, int *flag) {
    register int result asm("$2");
    register int flagValue asm("$3");

    asm volatile("lwc2 $0,0(%0)" : : "r"(v));
    asm volatile("lwc2 $1,4(%0)" : : "r"(v));
    asm volatile("nop");
    asm volatile(".word 0x4A180001");
    asm volatile("swc2 $14,0(%0)" : : "r"(sxy));
    asm volatile("swc2 $8,0(%0)" : : "r"(p));
    asm volatile("cfc2 %0,$31" : "=r"(flagValue));
    asm volatile("mfc2 %0,$19" : "=r"(result));
    *flag = flagValue;
    return result >> 2;
}

asm(".word 0x00000000");
