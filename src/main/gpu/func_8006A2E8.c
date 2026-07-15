extern unsigned short D_800BCE9E;
extern unsigned short D_800BCE8A;
extern unsigned char D_800B0DB1;

int func_8006A2E8(int arg0, unsigned int value) {
    register unsigned int v asm("$2");

    if (value < 16) {
        v = value;
        D_800BCE9E = v;
        D_800BCE8A = v;
        D_800B0DB1 = v;
    }
    return 0;
}
