/* MASPSX_FLAGS: --expand-div --stack-return-delay */

void SetDQA(int dqa);
void SetDQB(int dqb);

void Gte_SetDepthParams(int near, int far, int h) {
    register int range asm("$7");
    register int depth_scale asm("$2");
    register int dqa asm("$4");
    register int dqb_base asm("$3");
    register int dqb asm("$16");
    register int h_reg asm("$6");

    range = far - near;
    h_reg = h;
    if (range < 100) {
        return;
    }

    depth_scale = (-near * far) / range;
    asm volatile("" : : "r"(depth_scale) : "memory");
    dqb_base = (far << 12) / range;
    /* cc1 schedules a hazard nop after this mflo; the original does not. */
    asm volatile(
        ".set\tnoreorder\n\t"
        ".set noat\n\t"
        "sll\t$2,$2,8\n\t"
        "nop\n\t"
        "div\t$zero,$2,%2\n\t"
        "bnez\t%2,1f\n\t"
        "nop\n\t"
        "break\t7168\n"
        "1:\n\t"
        "addiu\t$at,$zero,-1\n\t"
        "bne\t%2,$at,2f\n\t"
        "lui\t$at,0x8000\n\t"
        "bne\t$2,$at,2f\n\t"
        "nop\n\t"
        "break\t6144\n"
        "2:\n\t"
        "mflo\t%0\n\t"
        ".set\tat\n\t"
        ".set\treorder"
        : "=r"(dqa)
        : "r"(depth_scale), "r"(h_reg)
        : "$1");
    dqb = dqb_base << 12;

    if (dqa < -0x8000) {
        dqa = -0x8000;
    }
    if (dqa > 0x7FFF) {
        dqa = 0x7FFF;
    }

    SetDQA(dqa);
    SetDQB(dqb);
}
