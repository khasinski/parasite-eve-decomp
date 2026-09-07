#include "pe1/gte.h"

typedef struct { short x, y, z, pad; } Vector;
typedef struct { short rotation[3][3]; int position[3]; } Matrix;
typedef struct { short y, z, shade; } Particle;
typedef struct { char reserved[0x3A]; short yaw; } Instance;
typedef struct { int reserved[2]; Instance *instance; } Actor;
extern Actor *D_800F32D0;
extern Matrix D_8019A870;
extern short D_8019A890, D_8019A892, D_8019A894;
extern int D_800E27EC, D_800966EC[], D_800F3428, D_8019A690[];
extern unsigned short D_800F336C, D_800E1204[];
extern short D_800F336A;
extern unsigned short GetClut(int, int);
extern void func_800CEE20(Vector *, Vector *, int, int, int, int, int, int, void *);

int func_801981D0(int event, Particle *particle)
{
    Vector position, rotation;
    if (event == 1) {
        int timer = D_800E27EC;
        if (timer >= 8) return 1;
        particle->shade = (short)D_800966EC[((unsigned int)timer << 8) & 0xF00] >> 5;
    } else if (event == 2) {
        int kind, palette;
        unsigned short clut;
        position.x = 0;
        position.y = particle->y;
        position.z = particle->z;
        {
            register Matrix *matrix asm("$8") = &D_8019A870;
            Vector *out;
            gte_ldrotmatrix(matrix);
            gte_ldtransmatrix(matrix);
            out = &position;
            gte_ldv0(out);
            gte_rtv0tr_mac();
            asm volatile("" : "=r"(out) : "0"(out));
            {
                register int x asm("$12");
                register int y asm("$13");
                register int z asm("$14");
                gte_mfc2_9(x); gte_mfc2_10(y); gte_mfc2_11(z);
                out->x = x; out->y = y; out->z = z;
            }
        }
        position.x += D_8019A890;
        position.y += D_8019A892;
        position.z += D_8019A894;
        rotation.x = (unsigned int)D_800E27EC * 384;
        rotation.y = D_800F32D0->instance->yaw;
        rotation.z = 0;
        rotation.pad = 1;
        kind = D_800F336C;
        palette = D_800E1204[kind];
        if (kind == 4 && D_800F3428) palette += 4;
        clut = GetClut(0, palette);
        {
            int timer = D_800E27EC;
            register int scale asm("$4") = D_800F336A;
            register int phase asm("$3") = timer & 7;
            register int product asm("$8") = scale * phase;
            int texture = product + 64;
            asm("" : : "r"(product), "r"(texture));
            func_800CEE20(&position, &rotation, 0x2200, 4096,
                texture, clut, 1, particle->shade, &D_8019A690[timer & 3]);
        }
    }
    return 0;
}
