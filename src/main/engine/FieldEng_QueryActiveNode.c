#include "pe1/field_anim.h"
#include "pe1/field_actor.h"

void Asset_Find08w(int arg0, int arg1, int arg2, int arg3, int arg4);

void func_800D3F64(int arg0, int arg1) {
    FieldAnimObjectPrefix *ctx;
    register FieldActor *child asm("$3");
    register RenderMatrix *node asm("$2");
    volatile short sx;
    volatile short sy;
    volatile short sz;
    int x;
    int y;
    int z;

    ctx = D_800F32D0;
    child = ctx->actor;
    node = child->render_object.matrices;
    x = node->translation[0];
    sx = x;
    asm volatile("" ::: "memory");
    x = (short)x;
    node = child->render_object.matrices;
    y = node->translation[1];
    sy = y;
    asm volatile("" ::: "memory");
    y = (short)y;
    node = child->render_object.matrices;
    z = node->translation[2];
    sz = z;
    z = (short)z;
    Asset_Find08w(arg0, arg1, x, y, z);
}


int func_800D3FD8(void) {
    FieldAnimObjectPrefix *ctx;
    FieldActorState *node;
    int value;

    ctx = D_800F32D0;
    node = ctx->actor->state;
    value = 0x80;
    if (node != 0) {
        value = node->progress;
        if (value >= 0x41) {
            value = 0x80;
        }
    }
    return value;
}
