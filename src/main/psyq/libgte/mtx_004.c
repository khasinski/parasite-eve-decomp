#include "pe1/gte_types.h"
#include "pe1/gte.h"

/* Split signed inputs at bit 15 before the two GTE rotation passes.
 * Empty outputs on architectural $0 retain retail's relative branches;
 * the final sign-dependent shifts deliberately preserve its wrapping path. */
GteVector *Gte_RotateVec(const GteMatrix *matrix, const GteVector *v, GteVector *out) {
    register s32 x asm("$8");
    register s32 y asm("$9");
    register s32 z asm("$10");
    register s32 hx asm("$11");
    register s32 hy asm("$12");
    register s32 hz asm("$13");
    register int zero asm("$0");
    register GteVector *dst asm("$6") = out;
    const GteMatrixWords *m = (const GteMatrixWords *)matrix;
    x = m->r11_r12;
    y = m->r13_r21;
    z = m->r22_r23;
    hx = m->r31_r32;
    hy = m->r33_pad;
    gte_ctc2_0(x);
    gte_ctc2_1(y);
    gte_ctc2_2(z);
    gte_ctc2_3(hx);
    gte_ctc2_4(hy);
    x = v->x;
    y = v->y;
    z = v->z;
    if (x >= 0) {
        goto positive_x;
    }
    x = 0U - x;
    hx = x >> 15;
    hx = 0U - hx;
    x &= 0x7fff;
    asm volatile("" : "=r"(zero));
    x = 0U - x;
    if (zero == 0) {
        goto split_x;
    }
positive_x:
    asm volatile("" : "=r"(x) : "0"(x));
    hx = x >> 15;
    x &= 0x7fff;
split_x:
    if (y >= 0) {
        goto positive_y;
    }
    y = 0U - y;
    hy = y >> 15;
    hy = 0U - hy;
    y &= 0x7fff;
    asm volatile("" : "=r"(zero));
    y = 0U - y;
    if (zero == 0) {
        goto split_y;
    }
positive_y:
    asm volatile("" : "=r"(y) : "0"(y));
    hy = y >> 15;
    y &= 0x7fff;
split_y:
    if (z >= 0) {
        goto positive_z;
    }
    z = 0U - z;
    hz = z >> 15;
    hz = 0U - hz;
    z &= 0x7fff;
    asm volatile("" : "=r"(zero));
    z = 0U - z;
    if (zero == 0) {
        goto split_z;
    }
positive_z:
    asm volatile("" : "=r"(z) : "0"(z));
    hz = z >> 15;
    z &= 0x7fff;
split_z:
    gte_mtc2_9(hx);
    gte_mtc2_10(hy);
    gte_mtc2_11(hz);
    gte_cop2_hazard_slot();
    gte_mvmva_rotation_ir_sf0();
    gte_mfc2_25(hx);
    gte_mfc2_26(hy);
    gte_mfc2_27(hz);
    gte_mtc2_9(x);
    gte_mtc2_10(y);
    gte_mtc2_11(z);
    gte_cop2_hazard_slot();
    gte_mvmva_rotation_ir_sf12();
    if (hx >= 0) {
        goto shift_positive_x;
    }
    hx = 0U - hx;
    hx = hx << 3;
    asm volatile("" : "=r"(zero));
    hx = 0U - hx;
    if (zero == 0) {
        goto shifted_x;
    }
shift_positive_x:
    hx = hx << 3;
shifted_x:
    if (hy >= 0) {
        goto shift_positive_y;
    }
    hy = 0U - hy;
    hy = hy << 3;
    asm volatile("" : "=r"(zero));
    hy = 0U - hy;
    if (zero == 0) {
        goto shifted_y;
    }
shift_positive_y:
    hy = hy << 3;
shifted_y:
    if (hz >= 0) {
        goto shift_positive_z;
    }
    hz = 0U - hz;
    hz = hz << 3;
    asm volatile("" : "=r"(zero));
    hz = 0U - hz;
    if (zero == 0) {
        goto shifted_z;
    }
shift_positive_z:
    hz = hz << 3;
shifted_z:
    gte_mfc2_25(x);
    gte_mfc2_26(y);
    gte_mfc2_27(z);
    x = x + hx;
    y = y + hy;
    z = z + hz;
    dst->x = x;
    dst->y = y;
    dst->z = z;
    asm volatile("" : "=r"(dst) : "0"(dst));
    return dst;
}
