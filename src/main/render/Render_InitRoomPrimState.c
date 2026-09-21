#include "pe1/render_lighting.h"
#include "pe1/render_object.h"
#include "pe1/gte.h"

static inline int ClampLight(int value)
{
    if (value < 0) return 0;
    if (value > 255) return 255;
    return value;
}

/* Build two opposite Y lights and upload their color matrix to the GTE. */
int Render_InitRoomPrimState(void *objectArg)
{
    RenderObjectEntity *object = objectArg;
    int intensity, r, g, b;

    D_800BEA40.matrix.m[0][0] = 0;
    D_800BEA42 = 4096;
    D_800BEA44 = 0;
    intensity = object->shade + object->lightPositiveY;
    r = D_800BD025;
    g = D_800BD026;
    b = D_800BD027;
    intensity = ClampLight(intensity) << 4;
    D_800BEA60.matrix.m[0][0] = intensity * r / 256;
    D_800BEA66 = intensity * g / 256;
    D_800BEA6C = intensity * b / 256;

    D_800BEA46 = 0;
    D_800BEA48 = -4096;
    D_800BEA4A = 0;
    intensity = object->shade + object->lightNegativeY;
    intensity = ClampLight(intensity) << 4;
    D_800BEA62 = intensity * r / 256;
    D_800BEA68 = intensity * g / 256;
    {
        int result = intensity * b / 256;
        asm("" : : "r"(result));
        D_800BEA6E = result;
    }
    D_800BEA4C = 0;
    D_800BEA4E = 0;
    D_800BEA50 = 0;
    D_800BEA64 = 0;
    D_800BEA6A = 0;
    D_800BEA70 = 0;
    {
        register u32 *matrix asm("$9") = D_800BEA60.words;
        register u32 a asm("$12");
        register u32 b asm("$13");
        register u32 c asm("$14");
        /* Commit the overlapping halfword aliases before reading words. */
        asm("" : "=r"(matrix) : "0"(matrix) : "memory");
        a = matrix[0];
        b = matrix[1];
        asm("" : : "r"(a), "r"(b));
        gte_ctc2_16(a);
        gte_ctc2_17(b);
        a = matrix[2];
        b = matrix[3];
        c = matrix[4];
        asm("" : : "r"(a), "r"(b), "r"(c));
        gte_ctc2_18(a);
        gte_ctc2_19(b);
        gte_ctc2_20(c);
    }
    return 0;
}
