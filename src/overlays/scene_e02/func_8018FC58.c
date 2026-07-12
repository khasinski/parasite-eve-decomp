typedef signed short s16;
typedef unsigned int u32;

typedef struct {
    char pad0[0x18];
    int x;
    int y;
    int z;
} FieldPos;

extern FieldPos *func_800C2B50(void);
extern int func_80077CF4(int angle);
extern int func_80077DC4(int angle);

void func_8018FC58(void *arg0, void *arg1, char *arg2) {
    register char *base asm("$20") = arg2;
    register FieldPos *pos asm("$19") = func_800C2B50();
    register u32 i asm("$18") = 0;
    register char *ptr asm("$17") = base;

    do {
        register int angle asm("$16") = i << 9;
        register int call_angle asm("$4") = angle;
        int z;

        *(s16 *)(ptr + 0x0) = pos->x;
        *(s16 *)(ptr + 0x2) = pos->y - 0x64;
        z = pos->z;
        i++;
        *(s16 *)(ptr + 0x4) = z;
        *(s16 *)(ptr + 0x80) = func_80077CF4(call_angle);
        *(s16 *)(ptr + 0x82) = 0;
        *(s16 *)(ptr + 0x84) = func_80077DC4(angle);
        ptr += 8;
    } while (i < 16);

    *(s16 *)(base + 0x102) = 0x80;
    *(s16 *)(base + 0x100) = 0x400;
    *(s16 *)(base + 0x108) = -0x3C;
    *(base + 0x105) = 0;
    *(s16 *)(base + 0x106) = 0x320;
}
