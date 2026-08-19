#ifndef PE1_COMMON_H
#define PE1_COMMON_H

typedef signed char s8;
typedef unsigned char u8;
typedef signed short s16;
typedef unsigned short u16;
typedef signed int s32;
typedef unsigned int u32;
typedef signed long long s64;
typedef unsigned long long u64;
typedef float f32;

/* C89-compatible compile-time assertion for the original GCC toolchain. Use a
 * descriptive, header-unique name so diagnostics identify the broken ABI. */
#define PE1_STATIC_ASSERT(expr, name) \
    typedef char pe1_static_assert_##name[(expr) ? 1 : -1]

#define PE1_OFFSETOF(type, member) ((u32)&(((type *)0)->member))

/*
 * Compiler scheduling helpers for byte-matching GCC 2.7.2 output. These emit
 * no CPU instructions; they only constrain register allocation or memory
 * motion when the original schedule depends on it.
 */
#define PE1_COMPILER_USE(value) \
    asm volatile("" : : "r"(value))

#define PE1_COMPILER_LAUNDER(value) \
    asm volatile("" : "=r"(value) : "0"(value))

#define PE1_COMPILER_LAUNDER_MEM(value) \
    asm volatile("" : "=r"(value) : "0"(value) : "memory")

#define PE1_COMPILER_LAUNDER2(value0, value1) \
    asm volatile("" : "=r"(value0), "=r"(value1) : "0"(value0), "1"(value1))

#define PE1_COMPILER_MEMORY_BARRIER() \
    asm volatile("" : : : "memory")

#endif
