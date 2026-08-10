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

#endif
