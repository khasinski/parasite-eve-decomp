extern unsigned short *g_IntrMaskRegPtr;

unsigned short SetIntrMask(unsigned short mask) {
    unsigned short prev = *g_IntrMaskRegPtr;
    *g_IntrMaskRegPtr = mask;
    return prev;
}
