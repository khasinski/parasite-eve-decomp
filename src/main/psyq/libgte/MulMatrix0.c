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
    const u32 *a = (const u32 *)left;
    const u32 *b = (const u32 *)right;
    u32 *c = (u32 *)out;
    xy = a[0];
    packed = a[1];
    z = a[2];
    firstX = a[3];
    firstY = a[4];
    gte_ctc2_0(xy);
    gte_ctc2_1(packed);
    gte_ctc2_2(z);
    gte_ctc2_3(firstX);
    gte_ctc2_4(firstY);
    /* Pack column zero as VXY0 and VZ0. All input columns precede output stores. */
    xy = (u16)right->m[0][0];
    packed = b[1];
    z = b[3];
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
    packed = b[2];
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
    packed = b[2];
    z = b[4];
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
    c[0] = secondX;
    asm volatile("" : "=r"(firstZ), "=r"(secondZ) : "0"(firstZ), "1"(secondZ), "m"(c[0]));
    firstZ &= 0xffff;
    secondZ <<= 16;
    secondZ |= firstZ;
    c[3] = secondZ;
    gte_mfc2_9(xy);
    gte_mfc2_10(packed);
    xy &= 0xffff;
    firstY <<= 16;
    xy |= firstY;
    c[1] = xy;
    asm volatile("" : "=r"(secondY), "=r"(packed) : "0"(secondY), "1"(packed), "m"(c[1]));
    secondY &= 0xffff;
    packed <<= 16;
    packed |= secondY;
    c[2] = packed;
    gte_swc2_11_16(out);
    ret = out;
    asm volatile("" : "=r"(ret) : "0"(ret));
    return ret;
}

/* Original trailing alignment words in this library object. */
static u32 tail_alignment_MulMatrix0[] __attribute__((section(".text"))) = { 0 };
