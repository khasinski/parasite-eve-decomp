typedef signed short s16;

void OuterProduct12(int *a, int *b, int *out);
void Gte_NormalizeVecS32toS16(int *in, void *out);

extern int D_800C213C[];

void func_800C65E4(s16 *from, s16 *to, char *out) {
    int delta[4];
    int crossA[4];
    int crossB[4];
    int basis[4];

    basis[0] = D_800C213C[0];
    basis[1] = D_800C213C[1];
    basis[2] = D_800C213C[2];
    basis[3] = D_800C213C[3];

    delta[0] = to[0] - from[0];
    delta[1] = to[1] - from[1];
    delta[2] = to[2] - from[2];

    OuterProduct12(delta, basis, crossA);
    OuterProduct12(delta, crossA, crossB);

    Gte_NormalizeVecS32toS16(crossA, out + 0x14);
    Gte_NormalizeVecS32toS16(crossB, out + 0x18);
    Gte_NormalizeVecS32toS16(delta, out + 0x1C);
}
