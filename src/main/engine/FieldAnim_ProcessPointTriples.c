#include "common.h"
#include "pe1/field_anim.h"
void func_800C2EAC(int arg0);
void func_800C3098(int arg0);
void func_800C2FF0(int arg0, int arg1);
void func_800C3238(int arg0);
void func_800C3B04(void *arg0);

void FieldAnim_ProcessPointTriples(void *arg0, void *arg1,
                                   FieldAnimPointData *points) {
    volatile int stack_pad;
    FieldAnimPointTriple *out;
    unsigned int i;

    i = 0;
    func_800C2EAC(3);
    func_800C3098(0x10);
    func_800C2FF0(0x10, 0x10);
    func_800C3238(0);

    D_800E2818.scale = (s32)(points->scale << 24) >> 23;

    if (points->count != 0) {
        out = &D_800E2818.point;
        /* This one-shot block preserves retail GCC's s1/s2 allocation. */
        do {
            do {
                out->x = points->x[i];
                out->y = points->y[i];
                out->z = points->z[i];
                func_800C3B04(out);
                i++;
            } while (i < points->count);
        } while (0);
    }
}
