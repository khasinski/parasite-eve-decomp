void RotTrans(void *v, void *out, int *flag) {
    register int flagValue asm("$2");

    asm volatile("lwc2 $0,0(%0)" : : "r"(v));
    asm volatile("lwc2 $1,4(%0)" : : "r"(v));
    asm volatile("nop");
    asm volatile(".word 0x4A480012");
    asm volatile("swc2 $25,0(%0)" : : "r"(out));
    asm volatile("swc2 $26,4(%0)" : : "r"(out));
    asm volatile("swc2 $27,8(%0)" : : "r"(out));
    asm volatile("cfc2 %0,$31" : "=r"(flagValue));
    *flag = flagValue;
}

asm(".word 0x00000000");
asm(".word 0x00000000");
