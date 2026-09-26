#include "menu_memcard_video.h"

void Memcard_UploadVideoSlice(void) {
    VideoRect rectangle;
    s8 old;
    s32 next;
    register s16 x asm("$2");
    register s16 width asm("$5");
    register s32 region asm("$6");
    register volatile u8 *buffer asm("$7");
    if (D_800B0DBB && D_800B0CD0) { func_8007C564(); D_800B0CD0=0; }
    {
        register VideoRect *source asm("$5") = (VideoRect *)&D_801D148C;
        rectangle = *source;
    }
    buffer = &D_801D1478;
    __asm__("" : "=r"(buffer) : "0"(buffer));
    old = *buffer;
    { register s32 selector asm("$3") = *buffer;
        __asm__("" : : : "memory");
        x = D_801D148C;
        width = D_801D1490;
        next = selector ^ 1;
    }
    *buffer = next;
    region = D_801D148A;
    x += width;
    D_801D148C = x;
    if (x < D_801D147A[region].x + D_801D147A[region].w) {
        { register s32 offset asm("$2") = next << 2;
            func_8010C01C(*(void **)((u8 *)buffer + offset - 8), width * D_801D1492 / 2); }
    } else {
        { register s32 one asm("$2") = 1;
            D_801D1494=one; }
        { register s32 index asm("$2") = region ^ 1;
            D_801D148A=index;
            __asm__("" : "=r"(index) : "0"(index) : "memory");
            D_801D148C=D_801D147A[index].x;
            __asm__("" : : : "memory");
            D_801D148E=D_801D147A[index].y;
        }
        if (D_801D0DC0 == 1) {
            s8 *mode = &D_800B0DBB;
            *mode ^= 1;
            if (*mode) D_801D1490=24;
            else D_801D1490=16;
            func_801918F8(D_8009CDDC ^ 1,D_800B0DBB);
            D_801D0DC0=2;
        }
    }
    func_8007506C(&rectangle,D_801D1470[old]);
}
