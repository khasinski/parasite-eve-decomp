extern int D_8009D2C0;
extern unsigned char D_8009D1C8;
extern unsigned char D_8009D1C9;
extern unsigned char D_8009D1CA;
extern unsigned char D_8009D1CB;

void func_8007A88C(void *arg0);

void Spu_SetVoiceVolume(int arg0) {
    register unsigned int value asm("$2");

    if (D_8009D2C0 & 2) {
        value = arg0;
        value <<= 1;
        value += arg0;
        value <<= 2;
        value -= arg0;
        value += value << 5;
        value <<= 3;
        value -= arg0;
        value >>= 13;
        D_8009D1CB = value;
        D_8009D1C9 = value;
        D_8009D1CA = value;
        D_8009D1C8 = value;
    } else {
        value = arg0;
        D_8009D1CA = value;
        D_8009D1C8 = value;
        D_8009D1CB = 0;
        D_8009D1C9 = 0;
    }
    func_8007A88C(&D_8009D1C8);
}
