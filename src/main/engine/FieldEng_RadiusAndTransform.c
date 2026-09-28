#include "common.h"
#include "pe1/gte_types.h"

int func_800C6148(char *obj, s16 *pos, unsigned short radius) {
    volatile int delta[3];
    int x = *(s16 *)(obj + 0x2A) - pos[0];
    int z;
    int x_sq = x * x;
    int z_sq;
    int radius_sq;
    delta[0] = x;
    z = *(s16 *)(obj + 0x32) - pos[2];
    z_sq = z * z;
    radius_sq = radius * radius;
    delta[2] = z;
    return x_sq + z_sq < radius_sq;
}

int func_800C653C(void *arg0, GteShortVector *verts);

extern GteShortVector D_800F3310[];

int func_800C61A8(void *arg0, char *matrix) {
    GteShortVector verts[4];

    ApplyMatrixSV(matrix, &D_800F3310[0], &verts[0]);
    ApplyMatrixSV(matrix, &D_800F3310[1], &verts[1]);
    ApplyMatrixSV(matrix, &D_800F3310[2], &verts[2]);
    ApplyMatrixSV(matrix, &D_800F3310[3], &verts[3]);

    verts[0].x += *(int *)(matrix + 0x14);
    verts[1].x += *(int *)(matrix + 0x14);
    verts[2].x += *(int *)(matrix + 0x14);
    verts[3].x += *(int *)(matrix + 0x14);

    verts[0].y = 0;
    verts[1].y = 0;
    verts[2].y = 0;
    verts[3].y = 0;

    verts[0].z += *(int *)(matrix + 0x1C);
    verts[1].z += *(int *)(matrix + 0x1C);
    verts[2].z += *(int *)(matrix + 0x1C);
    verts[3].z += *(int *)(matrix + 0x1C);

    return func_800C653C(arg0, verts) != 0;
}
