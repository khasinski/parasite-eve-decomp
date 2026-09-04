#include "common.h"
#include "pe1/gte_types.h"

extern u8 ROOMLIB_DRAW_EIGHT_SCALED_TABLE[];
extern u16 ROOMLIB_DRAW_EIGHT_SCALED_DEPTH;

char *func_800C2B50(void);
void func_800C2EAC(int mode);
void func_800C2FF0(int width, int height);
void func_800C3098(int value);
void func_800C3238(int mode);
void *memset(void *dest, int value, unsigned int size);
void Gte_ScaleMatrix(GteMatrix *matrix, int *scale);
void func_800C3134(void *table, int step, void *out);
void func_800C42A4(void *packet, GteMatrix *matrix, int mode);

void ROOMLIB_DRAW_EIGHT_SCALED_NAME(
    void *unused0, void *unused1, char *stateArg) {
    register char *state asm("$20");
    register unsigned int i asm("$18");
    register int unitScale asm("$19");
    register u16 *depthOut asm("$22");
    register char *packet asm("$21");
    register char *scalarCursor asm("$16");
    register char *positionCursor asm("$17");
    GteMatrix matrix;
    int scaleArg[4];
    volatile int sourceScale[4];
    GteMatrix *matrixArg;
    register int scale2 asm("$6");
    register int scale3 asm("$7");
    char *context;
    int secondScale;

    state = stateArg;
    context = func_800C2B50();
    i = 0;
    unitScale = 0x1000;
    depthOut = &ROOMLIB_DRAW_EIGHT_SCALED_DEPTH;
    packet = (char *)depthOut - 10;
    asm volatile("" ::: "memory");
    scalarCursor = state;
    asm volatile("" : "=r"(scalarCursor) : "0"(scalarCursor));
    positionCursor = scalarCursor;

    func_800C2EAC(context[0x44]);
    func_800C2FF0(0x20, 0x20);
    func_800C3098(0x10);
    func_800C3238(2);

    do {
        if (*(state + i + 0xB4) != 0) {
            matrix.m[2][2] = unitScale;
            matrix.m[1][1] = unitScale;
            matrix.m[0][0] = unitScale;
            matrix.t[2] = 0;
            matrix.t[1] = 0;
            matrix.t[0] = 0;
            matrix.m[2][1] = 0;
            matrix.m[2][0] = 0;
            matrix.m[1][2] = 0;
            matrix.m[1][0] = 0;
            matrix.m[0][2] = 0;
            matrix.m[0][1] = 0;

            memset((void *)sourceScale, 0, sizeof(sourceScale));
            sourceScale[0] = *(s16 *)(scalarCursor + 0x84);
            matrixArg = &matrix;
            asm volatile("" : "=r"(matrixArg) : "0"(matrixArg));
            secondScale = *(s16 *)(scalarCursor + 0x84);
            sourceScale[2] = unitScale;
            sourceScale[1] = secondScale;
            scaleArg[0] = sourceScale[0];
            scaleArg[1] = sourceScale[1];
            scale2 = sourceScale[2];
            scale3 = sourceScale[3];
            scaleArg[2] = scale2;
            scaleArg[3] = scale3;
            asm volatile("" ::: "$5");
            Gte_ScaleMatrix(matrixArg, scaleArg);

            *depthOut = *(u16 *)(scalarCursor + 0x94);
            matrix.t[0] = *(s16 *)(positionCursor + 4);
            matrix.t[1] = *(s16 *)(positionCursor + 6);
            matrix.t[2] = *(s16 *)(positionCursor + 8);
            func_800C3134(ROOMLIB_DRAW_EIGHT_SCALED_TABLE,
                          *(s16 *)(scalarCursor + 0xA4), packet);
            func_800C42A4(packet, &matrix, 1);
        }
        positionCursor += 8;
        i++;
        scalarCursor += 2;
    } while (i < 8);
}

#undef ROOMLIB_DRAW_EIGHT_SCALED_NAME
#undef ROOMLIB_DRAW_EIGHT_SCALED_TABLE
#undef ROOMLIB_DRAW_EIGHT_SCALED_DEPTH
