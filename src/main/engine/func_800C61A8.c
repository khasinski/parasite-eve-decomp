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
    int i;

    ApplyMatrixSV(matrix, &D_800F3310[0], &verts[0]);
    ApplyMatrixSV(matrix, &D_800F3310[1], &verts[1]);
    ApplyMatrixSV(matrix, &D_800F3310[2], &verts[2]);
    ApplyMatrixSV(matrix, &D_800F3310[3], &verts[3]);

    for (i = 0; i < 4; i++) {
        verts[i].vx += *(int *)(matrix + 0x14);
        verts[i].vy = 0;
        verts[i].vz += *(int *)(matrix + 0x1C);
    }

    return func_800C653C(arg0, verts) > 0;
}
