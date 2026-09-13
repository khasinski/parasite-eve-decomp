/* GAS_VERSION: 2.8.1 */
/* GCC_VERSION: 2.8.1 */

void SetDQA(int dqa);
void SetDQB(int dqb);

void Gte_SetDepthParams(int near, int far, int h) {
    int range;
    int depth_scale;
    int dqa;
    register int dqb_base asm("$3");
    int dqb;
    int h_reg;
    range = far - near;
    h_reg = h;
    if (range < 100) {
        return;
    }

    depth_scale = (-near * far) / range;
    asm volatile("" : : "r"(depth_scale) : "memory");
    dqb_base = (far << 12) / range;
    dqa = (depth_scale << 8) / h_reg;
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
