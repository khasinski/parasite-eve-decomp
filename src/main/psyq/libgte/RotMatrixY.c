/* ASSEMBLER: GNU */
#include "pe1/gte_types.h"
#include "pe1/psyq_nop.h"
extern u32 D_800966EC[];

/* Q12 rotation with retail wrap/rounding order. Pins, empty HI/LO and
 * scheduling constraints, and individual NOPs are tracked matching debt.
 * See proposals/PsyqRotationMatrix for the constraint rationale and evidence.
 */
GteMatrix *RotMatrixY(s32 angle, GteMatrix *m) {
    unsigned long long product;
    unsigned low;
    register s32 a asm("$15") = angle;
    register GteMatrix *ret = m;
    register u32 packed asm("$25");
    register u32 temp asm("$24");
    register s32 scratch asm("$14");
    register s32 cosine asm("$8");
    register s32 negative_sine asm("$9");
    register s32 x0 asm("$10");
    register s32 x1 asm("$11");
    register s32 x2 asm("$12");
    register s32 z0 asm("$13");
    register s32 z1 asm("$14");
    register s32 z2 asm("$15");
    asm volatile("" : "=r"(a), "=r"(ret) : "0"(a), "1"(ret));
    /* The second factor is -sin(angle), simplifying the row updates. */
    packed = (u32)a & 0xfff;
    if (a < 0) {
        a = 0U - (u32)a;
        /* INT_MIN wraps to table offset zero after the unsigned shift. */
        if (a < 0)
            asm volatile("" : : : "15");
        else
            a &= 0xfff;
        asm volatile("" : : : "24");
        temp = (u32)a << 2;
        packed = *(u32 *)((char *)D_800966EC + temp);
        scratch = packed << 16;
        asm volatile("" : "=r"(scratch) : "0"(scratch));
        negative_sine = scratch >> 16;
        cosine = (s32)packed >> 16;
    } else {
        temp = packed << 2;
        packed = *(u32 *)((char *)D_800966EC + temp);
        temp = packed << 16;
        asm volatile("" : "=r"(temp) : "0"(temp));
        a = (s32)temp >> 16;
        negative_sine = 0U - (u32)a;
        cosine = (s32)packed >> 16;
        asm volatile("" : "=r"(cosine) : "0"(cosine));
    }
    x0 = m->m[0][0];
    z0 = m->m[2][0];
    asm volatile("" : : "r"(x0), "r"(z0));
    product = (unsigned long long)(u32)cosine * (u32)x0;
    x1 = m->m[0][1];
    asm volatile("" : : "r"(x1) : "memory");
    z1 = m->m[2][1];
    asm volatile("" : : "x"(product), "r"(x1), "r"(z1));
    asm volatile("" : "=l"(low) : "x"(product));
    temp = low;
    asm volatile("" : "=r"(temp) : "0"(temp));
    x2 = m->m[0][2];
    asm volatile("" : : "r"(x2) : "memory");
    z2 = m->m[2][2];
    asm volatile("" : : "r"(temp), "r"(x2), "r"(z2));
    product = (unsigned long long)(u32)negative_sine * (u32)z0;
    PE1_NOP_DEP("x", product);
    PE1_NOP_DEP("x", product);
    asm volatile("" : "=l"(low) : "x"(product));
    packed = low;
    asm volatile("" : "=r"(temp), "=r"(packed) : "0"(temp), "1"(packed));
    packed = temp - packed;
    temp = (s32)packed >> 12;
    asm volatile("" : : "r"(temp));
    m->m[0][0] = temp;
    product = (unsigned long long)(u32)cosine * (u32)x1;
    PE1_NOP_DEP("x", product);
    asm volatile("" : "=l"(low) : "x"(product));
    temp = low;
    asm volatile("" : "=r"(temp) : "0"(temp));
    product = (unsigned long long)(u32)negative_sine * (u32)z1;
    PE1_NOP_DEP("x", product);
    PE1_NOP_DEP("x", product);
    asm volatile("" : "=l"(low) : "x"(product));
    packed = low;
    asm volatile("" : "=r"(temp), "=r"(packed) : "0"(temp), "1"(packed));
    packed = temp - packed;
    temp = (s32)packed >> 12;
    asm volatile("" : : "r"(temp));
    m->m[0][1] = temp;
    product = (unsigned long long)(u32)cosine * (u32)x2;
    PE1_NOP_DEP("x", product);
    asm volatile("" : "=l"(low) : "x"(product));
    temp = low;
    asm volatile("" : "=r"(temp) : "0"(temp));
    product = (unsigned long long)(u32)negative_sine * (u32)z2;
    PE1_NOP_DEP("x", product);
    PE1_NOP_DEP("x", product);
    asm volatile("" : "=l"(low) : "x"(product));
    packed = low;
    asm volatile("" : "=r"(temp), "=r"(packed) : "0"(temp), "1"(packed));
    packed = temp - packed;
    temp = (s32)packed >> 12;
    asm volatile("" : : "r"(temp));
    m->m[0][2] = temp;
    product = (unsigned long long)(u32)negative_sine * (u32)x0;
    PE1_NOP_DEP("x", product);
    asm volatile("" : "=l"(low) : "x"(product));
    temp = low;
    asm volatile("" : "=r"(temp) : "0"(temp));
    product = (unsigned long long)(u32)cosine * (u32)z0;
    PE1_NOP_DEP("x", product);
    PE1_NOP_DEP("x", product);
    asm volatile("" : "=l"(low) : "x"(product));
    packed = low;
    asm volatile("" : "=r"(temp), "=r"(packed) : "0"(temp), "1"(packed));
    packed = temp + packed;
    temp = (s32)packed >> 12;
    asm volatile("" : : "r"(temp));
    m->m[2][0] = temp;
    product = (unsigned long long)(u32)negative_sine * (u32)x1;
    PE1_NOP_DEP("x", product);
    asm volatile("" : "=l"(low) : "x"(product));
    temp = low;
    asm volatile("" : "=r"(temp) : "0"(temp));
    product = (unsigned long long)(u32)cosine * (u32)z1;
    PE1_NOP_DEP("x", product);
    PE1_NOP_DEP("x", product);
    asm volatile("" : "=l"(low) : "x"(product));
    packed = low;
    asm volatile("" : "=r"(temp), "=r"(packed) : "0"(temp), "1"(packed));
    packed = temp + packed;
    temp = (s32)packed >> 12;
    asm volatile("" : : "r"(temp));
    m->m[2][1] = temp;
    product = (unsigned long long)(u32)negative_sine * (u32)x2;
    PE1_NOP_DEP("x", product);
    asm volatile("" : "=l"(low) : "x"(product));
    temp = low;
    asm volatile("" : "=r"(temp) : "0"(temp));
    product = (unsigned long long)(u32)cosine * (u32)z2;
    PE1_NOP_DEP("x", product);
    PE1_NOP_DEP("x", product);
    asm volatile("" : "=l"(low) : "x"(product));
    packed = low;
    asm volatile("" : "=r"(temp), "=r"(packed) : "0"(temp), "1"(packed));
    packed = temp + packed;
    temp = (s32)packed >> 12;
    asm volatile("" : : "r"(temp));
    m->m[2][2] = temp;
    PE1_NOP();
    return ret;
}
