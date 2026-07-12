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
    register s16 *from_t0 asm("$8");
    register s16 *to_a3 asm("$7");
    register char *out_s2 asm("$18");
    register int *basis_src asm("$5");
    register int *delta_s1 asm("$17");
    register int *crossB_s0 asm("$16");
    register int *call_delta_a0 asm("$4");
    register int *call_basis_a1 asm("$5");
    register int *call_crossA_a2 asm("$6");
    register int b0 asm("$2");
    register int b1 asm("$3");
    register int b2 asm("$4");

    from_t0 = from;
    to_a3 = to;
    out_s2 = out;
    asm volatile("" ::: "$16", "$17");
    basis_src = D_800C213C;
    asm volatile("" : "=r"(basis_src) : "0"(basis_src));
    b0 = basis_src[0];
    b1 = basis_src[1];
    b2 = basis_src[2];
    basis[0] = b0;
    basis[1] = b1;
    basis[2] = b2;
    b0 = basis_src[3];
    asm volatile("" : "=r"(b0) : "0"(b0));
    basis[3] = b0;
    asm volatile("" ::: "memory");

    delta_s1 = delta;
    call_delta_a0 = delta_s1;
    delta_s1[0] = to_a3[0] - from_t0[0];
    call_basis_a1 = basis;
    delta_s1[1] = to_a3[1] - from_t0[1];
    call_crossA_a2 = crossA;
    delta_s1[2] = to_a3[2] - from_t0[2];

    OuterProduct12(call_delta_a0, call_basis_a1, call_crossA_a2);
    call_delta_a0 = delta_s1;
    call_basis_a1 = crossA;
    asm volatile("" : "=r"(call_basis_a1) : "0"(call_basis_a1));
    crossB_s0 = crossB;
    OuterProduct12(call_delta_a0, call_basis_a1, crossB_s0);

    Gte_NormalizeVecS32toS16(crossA, out_s2 + 0x14);
    Gte_NormalizeVecS32toS16(crossB_s0, out_s2 + 0x18);
    Gte_NormalizeVecS32toS16(delta_s1, out_s2 + 0x1C);
}
