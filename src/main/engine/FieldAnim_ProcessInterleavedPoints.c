#include "common.h"
#include "pe1/field_anim.h"

/* CC1_FLAGS: -fno-strength-reduce */

void func_800C2EAC(int arg0);
void func_800C3098(int arg0);
void func_800C2FF0(int arg0, int arg1);
void func_800C3238(int arg0);
void func_800C3B04(void *arg0);

#define FIELD_ANIM_EXTENT_OFFSET 6

void FieldAnim_ProcessInterleavedPoints(void *arg0, void *arg1, s8 *anim) {
    volatile int stack_pad;
    FieldAnimPointTriple *out;
    u8 *entry;
    unsigned int i;

    func_800C2EAC(3);
    func_800C3098(0x10);
    func_800C2FF0(0x20, 0x20);
    func_800C3238(2);

    D_800E2260.extent_x = *(short *)(anim + FIELD_ANIM_EXTENT_OFFSET);
    D_800E2260.extent_y = *(short *)(anim + FIELD_ANIM_EXTENT_OFFSET);
    D_800E2260.extent_z = *(short *)(anim + FIELD_ANIM_EXTENT_OFFSET);
    D_800E2260.scale = (u8)anim[1];

    if (anim[2] != 0) {
        i = 0;
        out = &D_800E2260.point;
        entry = (u8 *)anim;
        /* This one-shot block preserves retail GCC's s1/s2 allocation. */
        do {
            do {
                FieldAnimInterleavedWindow *window;

                window = (FieldAnimInterleavedWindow *)entry;
                out->x = window->point.x;
                out->y = window->point.y;
                out->z = window->point.z;
                func_800C3B04(out);
                i++;
                entry += 8;
            } while (i < anim[2]);
        } while (0);
    }
}
