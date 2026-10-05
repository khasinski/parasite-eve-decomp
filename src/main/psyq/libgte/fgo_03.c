/* ASSEMBLER: GNU */
#include "pe1/gte_types.h"
#include "pe1/gte_sine_table.h"
#include "pe1/psyq_nop.h"

/* Q12 rotation with retail wrap/rounding order. Pins, empty HI/LO and
 * scheduling constraints, and individual NOPs are tracked matching debt.
 * See proposals/PsyqRotationMatrix for the constraint rationale and evidence.
 */
GteMatrix *RotMatrixZYX(GteShortVector *angles, GteMatrix *matrix) {
    register GteMatrix *result;
    unsigned long long product;
    unsigned low;
    register s32 cx asm("$8");
    register s32 cy asm("$9");
    register s32 cz asm("$10");
    register s32 sx asm("$11");
    register s32 sy asm("$12");
    register s32 sz asm("$13");
    register s32 value asm("$14");
    register s32 angle asm("$15");
    register s32 temp asm("$24");
    register s32 word asm("$25");
    /* Decode packed sine/cosine for each signed angle. */
    angle = angles->x;
    result = matrix;
    word = angle & 0xFFF;
    if (angle >= 0)
        goto positive_x;
    angle = 0U - angle;
    if (angle < 0)
        asm volatile("" : : : "15");
    else
        angle &= 0xfff;
    asm volatile("" : : : "24");

    temp = angle << 2;
    word = *(u32 *)((char *)D_800966EC + temp);
    PE1_NOP_MEMORY_DEP("r", word);
    value = word << 16;
    value = value >> 16;
    sx = 0U - value;
    cx = word >> 16;
    goto x_ready;
positive_x:;
    temp = word << 2;
    word = *(u32 *)((char *)D_800966EC + temp);
    PE1_NOP_MEMORY_DEP("r", word);
    temp = word << 16;
    sx = temp >> 16;
    cx = word >> 16;
    asm volatile("" : "=r"(cx) : "0"(cx));
x_ready:;
    angle = angles->y;
    PE1_NOP_MEMORY_DEP("r", angle);
    word = angle & 0xFFF;
    if (angle >= 0)
        goto positive_y;
    angle = 0U - angle;
    if (angle < 0)
        asm volatile("" : : : "15");
    else
        angle &= 0xfff;
    asm volatile("" : : : "24");

    temp = angle << 2;
    word = *(u32 *)((char *)D_800966EC + temp);
    PE1_NOP_MEMORY_DEP("r", word);
    value = word << 16;
    value = value >> 16;
    sy = 0U - value;
    cy = word >> 16;
    goto y_ready;
positive_y:;
    temp = word << 2;
    word = *(u32 *)((char *)D_800966EC + temp);
    PE1_NOP_MEMORY_DEP("r", word);
    value = word << 16;
    sy = value >> 16;
    value = 0U - sy;
    cy = word >> 16;
    asm volatile("" : "=r"(cy) : "0"(cy));
y_ready:;
    product = (unsigned long long)(u32)sx * (u32)cy;
    angle = angles->z;
    matrix->m[2][0] = value;
    asm volatile("" : "=l"(low) : "x"(product));
    temp = low;
    asm volatile("" : "=r"(temp) : "0"(temp));
    value = temp >> 12;
    PE1_NOP_MEMORY_DEP("r", value);
    product = (unsigned long long)(u32)cx * (u32)cy;
    asm volatile("" : : "x"(product) : "memory");
    matrix->m[2][1] = value;
    word = angle & 0xFFF;
    if (angle >= 0)
        goto positive_z;
    asm volatile("" : "=l"(low) : "x"(product));
    temp = low;
    asm volatile("" : "=r"(temp) : "0"(temp));
    value = temp >> 12;
    matrix->m[2][2] = value;
    angle = 0U - angle;
    asm volatile("" : "=r"(angle) : "0"(angle));
    if (angle < 0)
        asm volatile("" : : : "15");
    else
        angle &= 0xfff;
    asm volatile("" : : : "24");

    temp = angle << 2;
    word = *(u32 *)((char *)D_800966EC + temp);
    PE1_NOP_MEMORY_DEP("r", word);
    temp = word << 16;
    temp = temp >> 16;
    sz = 0U - temp;
    cz = word >> 16;
    goto z_ready;
positive_z:;
    asm volatile("" : "=l"(low) : "x"(product));
    angle = low;
    asm volatile("" : "=r"(angle) : "0"(angle));
    value = angle >> 12;
    matrix->m[2][2] = value;
    asm volatile("" : : "m"(matrix->m[2][2]) : "memory");
    temp = word << 2;
    word = *(u32 *)((char *)D_800966EC + temp);
    PE1_NOP_MEMORY_DEP("r", word);
    temp = word << 16;
    sz = temp >> 16;
    cz = word >> 16;
    asm volatile("" : "=r"(cz) : "0"(cz));
z_ready:;
    product = (unsigned long long)(u32)cy * (u32)cz;
    PE1_NOP_MEMORY_DEP("x", product);
    PE1_NOP_MEMORY_DEP("x", product);
    asm volatile("" : "=l"(low) : "x"(product));
    angle = low;
    asm volatile("" : "=r"(angle) : "0"(angle));
    value = angle >> 12;
    matrix->m[0][0] = value;
    asm volatile("" : : "m"(matrix->m[0][0]) : "memory");
    product = (unsigned long long)(u32)sz * (u32)cy;
    PE1_NOP_MEMORY_DEP("x", product);
    PE1_NOP_MEMORY_DEP("x", product);
    asm volatile("" : "=l"(low) : "x"(product));
    angle = low;
    asm volatile("" : "=r"(angle) : "0"(angle));
    value = angle >> 12;
    PE1_NOP_MEMORY_DEP("r", value);
    product = (unsigned long long)(u32)sx * (u32)sy;
    matrix->m[1][0] = value;
    PE1_NOP_MEMORY_DEP("x", product);
    asm volatile("" : "=l"(low) : "x"(product));
    angle = low;
    asm volatile("" : "=r"(angle) : "0"(angle));
    temp = angle >> 12;
    PE1_NOP_MEMORY_DEP("r", temp);
    product = (unsigned long long)(u32)temp * (u32)cz;
    PE1_NOP_MEMORY_DEP("x", product);
    PE1_NOP_MEMORY_DEP("x", product);
    asm volatile("" : "=l"(low) : "x"(product));
    angle = low;
    asm volatile("" : "=r"(angle) : "0"(angle));
    value = angle >> 12;
    PE1_NOP_MEMORY_DEP("r", value);
    product = (unsigned long long)(u32)sz * (u32)cx;
    PE1_NOP_MEMORY_DEP("x", product);
    PE1_NOP_MEMORY_DEP("x", product);
    asm volatile("" : "=l"(low) : "x"(product));
    angle = low;
    asm volatile("" : "=r"(angle) : "0"(angle));
    word = angle >> 12;
    angle = value - word;
    asm volatile("" : "=r"(angle) : "0"(angle));
    product = (unsigned long long)(u32)cx * (u32)cz;
    matrix->m[0][1] = angle;
    PE1_NOP_MEMORY_DEP("x", product);
    asm volatile("" : "=l"(low) : "x"(product));
    value = low;
    asm volatile("" : "=r"(value) : "0"(value));
    angle = value >> 12;
    PE1_NOP_MEMORY_DEP("r", angle);
    product = (unsigned long long)(u32)temp * (u32)sz;
    PE1_NOP_MEMORY_DEP("x", product);
    PE1_NOP_MEMORY_DEP("x", product);
    asm volatile("" : "=l"(low) : "x"(product));
    value = low;
    asm volatile("" : "=r"(value) : "0"(value));
    word = value >> 12;
    value = word + angle;
    asm volatile("" : "=r"(value) : "0"(value));
    product = (unsigned long long)(u32)sy * (u32)cx;
    matrix->m[1][1] = value;
    PE1_NOP_MEMORY_DEP("x", product);
    asm volatile("" : "=l"(low) : "x"(product));
    angle = low;
    asm volatile("" : "=r"(angle) : "0"(angle));
    temp = angle >> 12;
    PE1_NOP_MEMORY_DEP("r", temp);
    product = (unsigned long long)(u32)temp * (u32)cz;
    PE1_NOP_MEMORY_DEP("x", product);
    PE1_NOP_MEMORY_DEP("x", product);
    asm volatile("" : "=l"(low) : "x"(product));
    angle = low;
    asm volatile("" : "=r"(angle) : "0"(angle));
    value = angle >> 12;
    PE1_NOP_MEMORY_DEP("r", value);
    product = (unsigned long long)(u32)sx * (u32)sz;
    PE1_NOP_MEMORY_DEP("x", product);
    PE1_NOP_MEMORY_DEP("x", product);
    asm volatile("" : "=l"(low) : "x"(product));
    angle = low;
    asm volatile("" : "=r"(angle) : "0"(angle));
    word = angle >> 12;
    angle = value + word;
    asm volatile("" : "=r"(angle) : "0"(angle));
    product = (unsigned long long)(u32)sx * (u32)cz;
    matrix->m[0][2] = angle;
    PE1_NOP_MEMORY_DEP("x", product);
    asm volatile("" : "=l"(low) : "x"(product));
    value = low;
    asm volatile("" : "=r"(value) : "0"(value));
    angle = value >> 12;
    PE1_NOP_MEMORY_DEP("r", angle);
    product = (unsigned long long)(u32)temp * (u32)sz;
    PE1_NOP_MEMORY_DEP("x", product);
    PE1_NOP_MEMORY_DEP("x", product);
    asm volatile("" : "=l"(low) : "x"(product));
    value = low;
    asm volatile("" : "=r"(value) : "0"(value));
    word = value >> 12;
    value = word - angle;
    matrix->m[1][2] = value;
    PE1_NOP_MEMORY_DEP("r", value);
    return result;
}
