extern unsigned short *g_IntrMaskRegPtr;

unsigned short GetIntrMask(void) {
    return *g_IntrMaskRegPtr;
}
