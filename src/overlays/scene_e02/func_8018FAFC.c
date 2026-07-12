typedef signed short s16;

typedef struct {
    char pad0[0x18];
    int x;
    int y;
    int z;
} FieldPos;

typedef struct {
    s16 x;
    s16 y;
    s16 z;
    char pad6[0x2];
    s16 h8;
    s16 hA;
} SceneE02VecState;

extern FieldPos *func_800C2B50(void);

void func_8018FAFC(void *arg0, void *arg1, SceneE02VecState *arg2) {
    FieldPos *pos = func_800C2B50();
    int z;

    arg2->x = pos->x;
    arg2->y = pos->y;
    z = pos->z;
    arg2->hA = 0;
    arg2->h8 = 0x400;
    arg2->z = z;
}
