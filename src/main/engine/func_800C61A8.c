typedef signed short s16;

typedef struct {
    s16 vx;
    s16 vy;
    s16 vz;
    s16 pad;
} SVECTOR;

void ApplyMatrixSV(void *matrix, SVECTOR *in, SVECTOR *out);
int func_800C653C(void *arg0, SVECTOR *verts);

extern SVECTOR D_800F3310[];

int func_800C61A8(void *arg0, char *matrix) {
    SVECTOR verts[4];

    ApplyMatrixSV(matrix, &D_800F3310[0], &verts[0]);
    ApplyMatrixSV(matrix, &D_800F3310[1], &verts[1]);
    ApplyMatrixSV(matrix, &D_800F3310[2], &verts[2]);
    ApplyMatrixSV(matrix, &D_800F3310[3], &verts[3]);

    verts[0].vx += *(int *)(matrix + 0x14);
    verts[1].vx += *(int *)(matrix + 0x14);
    verts[2].vx += *(int *)(matrix + 0x14);
    verts[3].vx += *(int *)(matrix + 0x14);

    verts[0].vy = 0;
    verts[1].vy = 0;
    verts[2].vy = 0;
    verts[3].vy = 0;

    verts[0].vz += *(int *)(matrix + 0x1C);
    verts[1].vz += *(int *)(matrix + 0x1C);
    verts[2].vz += *(int *)(matrix + 0x1C);
    verts[3].vz += *(int *)(matrix + 0x1C);

    return func_800C653C(arg0, verts) != 0;
}
