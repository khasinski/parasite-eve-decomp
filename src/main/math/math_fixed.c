/* CC1_PROFILE: build0001 */
void SetDrawMode(void *p, int dfe, int dtd, int tpage);
void SetSprt(unsigned char *arg0);
int Gpu_AppendPacketPrimitive(void *arg0, void *arg1);
void exit(int code);

int Math_FixedDivide(int a, int b) {
    return (a / (b >> 8)) << 8;
}

int Math_FixedRoundToInt(int arg0) {
    register int bias asm("$1");
    int value;

    bias = 0x8000;
    asm volatile("add\t%0,%1,%2" : "=r"(value) : "r"(arg0), "r"(bias));
    return value >> 16;
}

int Math_FixedRoundToByte(int arg0) {
    register int bias asm("$1");
    int value;

    bias = 0x8000;
    asm volatile("add\t%0,%1,%2" : "=r"(value) : "r"(arg0), "r"(bias));
    return value >> 8;
}

void Gpu_InitDrawModeSprtPacket(void *packet, int tpage) {
    void *prim = (char *)packet + 8;

    SetDrawMode(packet, 0, 1, tpage);
    SetSprt(prim);
    if (Gpu_AppendPacketPrimitive(packet, prim) != 0) {
        exit(-1);
    }
}
