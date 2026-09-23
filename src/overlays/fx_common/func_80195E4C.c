typedef struct {
    int x, y, z;
} FxVec3;

typedef struct {
    int x, y;
} FxVec2;

extern char D_801D0260[];
extern int D_8019C810, D_8019C814, D_8019C818;
extern int D_8019C330, D_8019C334, D_8019C338;
extern short D_8019C054, D_8019C050, D_8019C02C;
extern volatile unsigned char D_8019C040;
extern unsigned char D_8019C041, D_8019C042;

extern int func_8006EC6C(void *, int);
extern int func_8018F55C(int, int, int, volatile FxVec3 *, FxVec2 *);

void func_80195E4C(int index, int unused1, int unused2, int source)
{
    volatile FxVec3 point;
    FxVec2 scratch;
    register int rawIndex asm("$17") = index;
    short savedIndex = rawIndex;
    int lookup;
    int result;
    int shortIndex;

    lookup = func_8006EC6C(D_801D0260, 2);
    shortIndex = savedIndex;
    func_8018F55C(source, shortIndex + 1, lookup, &point, &scratch);
    D_8019C054 = 0;
    D_8019C810 = point.x;
    D_8019C814 = point.y;
    D_8019C818 = point.z;

    lookup = func_8006EC6C(D_801D0260, 2);
    result = func_8018F55C(source, shortIndex, lookup, &point, &scratch);
    D_8019C040 = result - 2;
    D_8019C050 = 0;
    D_8019C041 = 0;
    D_8019C042 = rawIndex;
    D_8019C02C = 0;
    D_8019C330 = point.x;
    D_8019C334 = point.y;
    D_8019C338 = point.z;
}
