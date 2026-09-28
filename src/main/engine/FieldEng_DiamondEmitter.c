#include "pe1/render_object.h"
#include "pe1/render_prim.h"
#include "pe1/field_anim.h"
#include "pe1/field_actor.h"
#include "pe1/gte.h"
#include "pe1/psyq_nop.h"

void func_800DB25C(GteShortVector *position, int x, int y, int angle,
                   int radius, int depth, int intensity, RenderColor *color)
{
    GteShortVector center, p0, p1, p2, p3;
    RenderLinePacket *line;
    int r, g, b, dx, dy;
    line = (RenderLinePacket *)(D_800B0E38.packets[D_8009CDDC] + D_8009CDD8);
    D_8009CDD8 += 64;
    gte_lwc2_0_0(position);
    gte_lwc2_1_4(position);
    PE1_NOP();
    PE1_NOP();
    gte_rtps_command();
    r = color->r * intensity / 128;
    g = color->g * intensity / 128;
    b = color->b * intensity / 128;
    gte_stsxy2(&center);
    center.x += x;
    center.y += y;
    dx = rcos(angle) * radius / 4096;
    dy = rsin(angle) * radius / 4096;
    p0.x = center.x + dx;
    p0.y = center.y + dy;
    p2.x = center.x - dx;
    p2.y = center.y - dy;
    angle += 1024;
    dx = rcos(angle) * radius / 4096;
    dy = rsin(angle) * radius / 4096;
    p1.x = center.x + dx;
    p1.y = center.y + dy;
    p3.x = center.x - dx;
    p3.y = center.y - dy;
#define EDGE(a, z) \
    SetLineF2(line); \
    line->r = r; line->g = g; line->b = b; \
    line->x0 = a.x; line->y0 = a.y; \
    line->x1 = z.x; line->y1 = z.y; \
    func_800CF6F8(D_800B0E38.ordering[D_8009CDDC] + 16, line, depth)
    EDGE(p0, p1);
    line++;
    EDGE(p1, p2);
    line++;
    EDGE(p2, p3);
    line++;
    EDGE(p3, p0);
#undef EDGE
}

int func_800DB5F4(int mode, RenderDiamondParticle *state)
{
    RenderColor color;
    int fade;
    switch (mode) {
    case 1:
        if (D_800E27EC < 8) break;
        return 1;
    case 2:
        fade = 128 - (D_800E27EC << 4);
        func_800CF3AC(D_800E1CC8, &color, state->color_time);
        func_800DB25C(&D_800E2224, state->x, state->y, state->angle, state->size, 1, fade, &color);
        break;
    }
    return 0;
}

/* Splitting signed /24 keeps the multiply-high result explicit. The pins
 * avoid GCC 2.7.2 reload reserving $9 when the GTE block uses $8. */
static __inline__ int divide24_radius(int value)
{
    register int high asm("$8") = value / 6 + (value >> 31);
    register int result asm("$5") = high >> 2;
    return result - (value >> 31);
}
static __inline__ int divide24_angle(int value)
{
    register int high asm("$8") = value / 6 + (value >> 31);
    register int result asm("$2") = high >> 2;
    return result - (value >> 31);
}
int func_800DB6BC(int mode, RenderDiamondEmitter *state)
{
    RenderColor color;
    RenderDiamondParticle *particle;
    int phase;
    register int x asm("$12");
    register int y asm("$13");
    register int z asm("$14");
    s32 **slot;
    switch (mode) {
    case 0:
        state->position.x = D_800F32D0->actor->render_object.target_x;
        state->position.y = D_800F32D0->actor->render_object.target_y;
        state->position.z = D_800F32D0->actor->render_object.target_z;
        return func_800CE560(D_800F33E0->end, 12, 10, (FieldAnimTaskCallback)func_800DB5F4);
    case 1:
        if (D_800E27EC < 25) {
            state->angle = -D_800E27EC * 64;
            state->radius = 150 - divide24_radius(D_800E27EC * 150);
            state->x = rcos(state->angle * 2) * state->radius / 4096;
            state->y = rsin(state->angle * 2) * state->radius / 4096;
            state->size = rcos(divide24_angle(D_800E27EC << 10)) * 80 / 4096 + 32;
            particle = func_800CE610(D_800F33E0->end);
            if (particle) {
                particle->x = state->x;
                particle->y = state->y;
                particle->angle = state->angle;
                particle->size = state->size;
            }
        } else {
            state->y = 0;
            state->x = 0;
        }
        if (D_800E27EC >= 72) return 1;
        break;
    case 2:
        {
            register GteMatrixWords *matrix asm("$8");
            slot = &D_800BCFA4.value;
            asm volatile("" : "=r"(slot) : "0"(slot));
            matrix = (GteMatrixWords *)*slot;
            x = matrix->r11_r12;
            y = matrix->r13_r21;
            gte_ctc2_0(x);
            gte_ctc2_1(y);
            x = matrix->r22_r23;
            y = matrix->r31_r32;
            z = matrix->r33_pad;
            gte_ctc2_2(x);
            gte_ctc2_3(y);
            gte_ctc2_4(z);
            x = matrix->tx;
            y = matrix->ty;
            gte_ctc2_5(x);
            z = matrix->tz;
            gte_ctc2_6(y);
            gte_ctc2_7(z);
        }

        D_800E2224.x = state->position.x;
        D_800E2224.y = state->position.y;
        D_800E2224.z = state->position.z;
        func_800CF3AC(D_800E1CC8, &color, D_800E27EC);
        func_800DB25C(&D_800E2224, state->x, state->y, state->angle, state->size, 1, 128, &color);
        phase = D_800E27EC - 24;
        if ((unsigned)phase < 16) {
            int angle = phase * 64;
            func_800DB25C(&D_800E2224, state->x, state->y, state->angle, state->size + rcos(angle)/16, 1, 64, &color);
            func_800DB25C(&D_800E2224, state->x, state->y, state->angle, state->size + rcos(angle)/32, 1, 96, &color);
        }
        break;
    }
    return 0;
}
