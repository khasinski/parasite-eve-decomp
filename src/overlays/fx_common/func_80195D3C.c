typedef struct {
    int x, y, z;
} FxVec3;

typedef struct {
    int x, y;
} FxVec2;

extern char D_801D0260[];
extern unsigned char D_8019C040, D_8019C041, D_8019C042;
extern int D_8019C810, D_8019C814, D_8019C818;
extern int D_8019C330, D_8019C334, D_8019C338;

extern int func_8006EC6C(void *, int);
extern int func_8018F55C(int, int, int, FxVec3 *, FxVec2 *);

void func_80195D3C(void)
{
    FxVec3 point;
    FxVec2 scratch;
    int lookup;

    if (D_8019C040 > 1) {
        D_8019C040--;
        D_8019C041++;

        lookup = func_8006EC6C(D_801D0260, 2);
        func_8018F55C(D_8019C041 << 8, D_8019C042 + 1,
                       lookup, &point, &scratch);
        D_8019C810 = point.x;
        D_8019C814 = point.y;
        D_8019C818 = point.z;

        lookup = func_8006EC6C(D_801D0260, 2);
        func_8018F55C(D_8019C041 << 8, D_8019C042,
                       lookup, &point, &scratch);
        D_8019C330 = point.x;
        D_8019C334 = point.y;
        D_8019C338 = point.z;
    }
}
