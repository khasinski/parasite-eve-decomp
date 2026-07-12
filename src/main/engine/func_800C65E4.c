/* CC1_FLAGS: -fno-schedule-insns */
typedef signed short s16;

void OuterProduct12(int *a, int *b, int *out);
void Gte_NormalizeVecS32toS16(int *in, void *out);

extern int D_800C213C[];

void func_800C65E4(s16 *from, s16 *to, char *out) {
    int crossA[4];
    int crossB[4];
    int delta[4];
    int basis[4];
    int *basis_src = D_800C213C;

    basis[0] = basis_src[0];
    basis[1] = basis_src[1];
    basis[2] = basis_src[2];
    basis[3] = basis_src[3];

    delta[0] = to[0] - from[0];
    delta[1] = to[1] - from[1];
    delta[2] = to[2] - from[2];

    OuterProduct12(delta, basis, crossA);
    OuterProduct12(delta, crossA, crossB);

    Gte_NormalizeVecS32toS16(crossA, out + 0x14);
    Gte_NormalizeVecS32toS16(crossB, out + 0x18);
    Gte_NormalizeVecS32toS16(delta, out + 0x1C);
}
