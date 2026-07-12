typedef signed short s16;
typedef unsigned int u32;

typedef struct {
    char pad0[0x18];
    int x;
    int y;
    int z;
} FieldPos;

typedef struct {
    s16 vecs[0x40];
    s16 h80;
    s16 h82;
} SceneE02VecSet;

extern FieldPos *func_800C2B50(void);

void func_80190054(void *arg0, void *arg1, SceneE02VecSet *arg2) {
    register FieldPos *pos asm("$5") = func_800C2B50();
    register u32 i asm("$4") = 0;
    register char *vec asm("$3") = (char *)arg2;

    do {
        *(s16 *)(vec + 0) = pos->x;
        *(s16 *)(vec + 2) = pos->y;
        *(s16 *)(vec + 4) = pos->z;
        i++;
        vec += 8;
    } while (i < 16);

    arg2->h82 = 0x80;
    arg2->h80 = 0x800;
}
