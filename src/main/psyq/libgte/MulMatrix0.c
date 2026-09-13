#include "pe1/gte.h"
#include "pe1/gte_types.h"

GteMatrix *MulMatrix0(const GteMatrix *left, const GteMatrix *right, GteMatrix *out) {
    register u32 xy asm("$8");
    register u32 packed asm("$9");
    register u32 z asm("$10");
    register u32 firstX asm("$11");
    register u32 firstY asm("$12");
    register u32 firstZ asm("$13");
    register u32 secondX asm("$14");
    register u32 secondY asm("$15");
    register u32 secondZ asm("$24");
    register u32 mask asm("$1");
    GteMatrix *ret;
    const GteMatrixWords *a = (const GteMatrixWords *)left;
    const GteMatrixWords *b = (const GteMatrixWords *)right;
    GteMatrixWords *c = (GteMatrixWords *)out;
    xy = a->r11_r12;
    packed = a->r13_r21;
    z = a->r22_r23;
    firstX = a->r31_r32;
    firstY = a->r33_pad;
    gte_ctc2_0(xy);
    gte_ctc2_1(packed);
    gte_ctc2_2(z);
    gte_ctc2_3(firstX);
    gte_ctc2_4(firstY);
    /* Pack column zero as VXY0 and VZ0. All input columns precede output stores. */
    xy = (u16)right->m[0][0];
    packed = b->r13_r21;
    z = b->r31_r32;
    asm volatile("" : "=r"(xy), "=r"(packed), "=r"(z) : "0"(xy), "1"(packed), "2"(z));
    mask = 0xffff0000U;
    asm volatile("" : "=r"(mask) : "0"(mask));
    packed &= mask;
    xy |= packed;
    gte_mtc2_0(xy);
    gte_mtc2_1(z);
    gte_cop2_hazard_slot();
    gte_mvmva_rotation_v0_sf12();
    /* Start column one while retaining column zero in scalar registers. */
    xy = (u16)right->m[0][1];
    packed = b->r22_r23;
    z = right->m[2][1];
    asm volatile("" : "=r"(xy), "=r"(packed), "=r"(z) : "0"(xy), "1"(packed), "2"(z));
    packed <<= 16;
    xy |= packed;
    gte_mfc2_9(firstX);
    gte_mfc2_10(firstY);
    gte_mfc2_11(firstZ);
    gte_mtc2_0(xy);
    gte_mtc2_1(z);
    gte_cop2_hazard_slot();
    gte_mvmva_rotation_v0_sf12();
    /* Start column two while retaining column one. */
    xy = (u16)right->m[0][2];
    packed = b->r22_r23;
    z = b->r33_pad;
    asm volatile("" : "=r"(xy), "=r"(packed), "=r"(z) : "0"(xy), "1"(packed), "2"(z));
    mask = 0xffff0000U;
    asm volatile("" : "=r"(mask) : "0"(mask));
    packed &= mask;
    xy |= packed;
    gte_mfc2_9(secondX);
    gte_mfc2_10(secondY);
    gte_mfc2_11(secondZ);
    gte_mtc2_0(xy);
    gte_mtc2_1(z);
    gte_cop2_hazard_slot();
    gte_mvmva_rotation_v0_sf12();
    firstX &= 0xffff;
    secondX <<= 16;
    secondX |= firstX;
    c->r11_r12 = secondX;
    asm volatile("" : "=r"(firstZ), "=r"(secondZ) : "0"(firstZ), "1"(secondZ), "m"(c->r11_r12));
    firstZ &= 0xffff;
    secondZ <<= 16;
    secondZ |= firstZ;
    c->r31_r32 = secondZ;
    gte_mfc2_9(xy);
    gte_mfc2_10(packed);
    xy &= 0xffff;
    firstY <<= 16;
    xy |= firstY;
    c->r13_r21 = xy;
    asm volatile("" : "=r"(secondY), "=r"(packed) : "0"(secondY), "1"(packed), "m"(c->r13_r21));
    secondY &= 0xffff;
    packed <<= 16;
    packed |= secondY;
    c->r22_r23 = packed;
    gte_swc2_11_16(out);
    ret = out;
    asm volatile("" : "=r"(ret) : "0"(ret));
    return ret;
}

/* Original trailing alignment words in this library object. */
static u32 tail_alignment_MulMatrix0[] __attribute__((section(".text"))) = { 0 };
