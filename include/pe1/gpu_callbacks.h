#ifndef PE1_GPU_CALLBACKS_H
#define PE1_GPU_CALLBACKS_H

/* Canonical libgpu callback/dispatch table shared by the gpu/*.c helpers.
 * Every slot is a 32-bit word (lw/sw); fn-ptr slots use unprototyped void(*)()
 * so any assignment/call compiles with a byte-identical store. Offsets 0x18/
 * 0x1C/0x20 are reused as either a fn-ptr or a data pointer across files, so
 * they are named unions. Total size is not relied upon (no slot is used with
 * array/pointer-arithmetic stride). */
typedef struct GpuCallbacks {
    /* 0x00 */ void *field_00;
    /* 0x04 */ void *field_04;
    /* 0x08 */ int (*addque2)();
    /* 0x0C */ void *clr;
    /* 0x10 */ int (*callback10)();
    /* 0x14 */ int (*cb14)();
    /* 0x18 */ union { int (*moveImage)(); void *cwc; void *packet; } u18;
    /* 0x1C */ union { int (*store)(); void *drs; } u1c;
    /* 0x20 */ union { int (*load)(); void *dws; } u20;
    /* 0x24 */ unsigned char pad_24[0x10];
    /* 0x34 */ int (*reset)();
    /* 0x38 */ int (*callback)();
    /* 0x3C */ int (*callback3C)();
} GpuCallbacks;

#endif /* PE1_GPU_CALLBACKS_H */
