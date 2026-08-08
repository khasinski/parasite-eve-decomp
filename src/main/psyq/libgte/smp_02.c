int RotTransPers(void *v, void *sxy, void *p, int *flag) {
    int result;
    int flagValue;
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
