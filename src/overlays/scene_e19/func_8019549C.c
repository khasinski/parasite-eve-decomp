#include "pe1/render_object.h"
#include "pe1/gte.h"
#include "pe1/psyq_gpu.h"
#include "pe1/field_anim.h"
#include "pe1/field_actor.h"
#include "pe1/overlay_math.h"
#include "pe1/random.h"

/* Full callback reconstruction; source-level register constraints are tracked
 * in crutch debt because stock GCC needs them to reproduce the retail code. */
typedef struct SceneE19HomingParticle {
    GteShortVector position; /* position.pad is the particle index */
    GteShortVector angles;
    GteShortVector target;
    s16 speed;
    s16 turnPhase;
    s16 state;
    s16 timer;
} SceneE19HomingParticle;

PE1_STATIC_ASSERT(sizeof(SceneE19HomingParticle) == 0x20,
                  scene_e19_homing_particle_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(SceneE19HomingParticle, state) == 0x1C,
                  scene_e19_homing_particle_state);

/* The original copies all eight bytes using unaligned word transfers. */
typedef struct SceneE19PackedPoint {
    GteShortVector vector;
} __attribute__((packed)) SceneE19PackedPoint;

extern SceneE19PackedPoint D_8018F210;
extern FieldActor *D_8009D254;
int func_800CFE94(s16 *from, s16 *to);
void func_800CFD50(u16 *from, u16 *to, u16 speed);
int func_800C6B90(s16 *position, int extraRadius);

extern s16 D_800942EC, D_800F3376, D_800F3378;
extern RenderColor D_8018F218;
/* Not declared in production headers; parameter order inferred from both
 * call sites and the ABI. The eighth argument is zero at both sites. */
extern void func_800D3BC8(GteShortVector *, int, int, int,
                         int, int, int, int, int);


int func_8019549C(int mode, SceneE19HomingParticle *particle)
{
    SceneE19PackedPoint templatePoint = D_8018F210;
    GteShortVector temporaryPosition;
    GteShortVector rotation;
    RenderColor color = D_8018F218;
    GteShortVector floor_position;
    GteRotation floor_rotation;
    register int divider asm("$16");
    int bias, random;
    register int remMagic asm("$5");
    register int test asm("$2");
    register int sum asm("$3");
    int intensity, scale, palette, pulseSum;
    u16 clut;
    u16 floorY;
    u16 floorZ;
    register int floorIndex asm("$4");
    int floorTimer;
    register int negValue asm("$2");
    int signedValue;
    register int returnTest asm("$2");

    switch (mode) {
    case 1:
        switch (particle->state) {
    case 0:
        particle->timer = 0;
        particle->state = 1;
        particle->angles.pad = 0;
        func_800CE8F0(
            (struct RoomFxTransformOwner *)D_800F32D0->actor,
            0, &templatePoint.vector, &particle->position);
        func_800CE9D4((struct RoomFxTransformOwner *)D_800F32D0->actor,
                     0, &rotation);
        rotation.x = 0;
        func_800CE870((char *)D_8009D254, 1, &particle->target.x);
        func_800CE870((char *)D_8009D254, 0, &temporaryPosition.x);

        test = func_800CFE94(&particle->position.x, &temporaryPosition.x) < 1900;
        test = -test;
        bias = test & 500;
        random = func_80071A54();
        remMagic = 0x66660000;
        asm("" : : "r"(remMagic));
        random &= 1023;
        asm volatile("" : : "r"(random) : "memory");
        sum = bias + 390;
        rotation.x += random - sum;
        asm("" : : "r"(sum));
        rotation.y += -512 + (s16)(particle->position.pad % 5) * 256;
        particle->angles.x = rotation.x;
        particle->angles.y = rotation.y;
        particle->angles.z = rotation.z;
        particle->speed = 24;
        particle->turnPhase = 0;
        asm("" : : : "$3", "$18", "$9", "$10", "$11", "$15", "$24", "$25");
        break;

    case 1:
        particle->timer++;
        func_800CFAA8(&particle->position, &particle->target,
                                    &rotation);
        func_800CFD50((u16 *)&rotation, (u16 *)&particle->angles,
                     (u16)(func_80077CF4(particle->turnPhase) / 5));

        if (particle->speed < 62)
            particle->speed += 4;
        if (particle->speed >= 40) {
            if (particle->turnPhase < 2048)
                particle->turnPhase += 102;
            if (particle->turnPhase > 2048)
                particle->turnPhase = 2048;
        }

        func_800CFB7C(&particle->angles,
                     (s16)(particle->speed * particle->speed / 16),
                     &temporaryPosition);
        particle->position.x += temporaryPosition.x;
        particle->position.y += temporaryPosition.y;
        particle->position.z += temporaryPosition.z;

        if (func_800C6B90(&particle->position.x, 190) &&
            particle->position.y > D_800942EC - 514) {
            if (D_800E2368->flags &&
                (D_800F32D0->actor->state->core_flags & 0x3F000000)
                    == 0x01000000) {
                D_8009D254->state->flags |= 0x4000;
                D_800F32D0->actor->state->core_flags =
                    (D_800F32D0->actor->state->core_flags & 0xC0FFFFFF)
                    | 0x29000000;
                D_800F32D0->actor->state->core_flags |= 0x80000000;
            }
            particle->state = 2;
            particle->timer = 0;
        }

        if (particle->position.y >= D_800942EC) {
            particle->position.y = D_800942EC;
            particle->state = 2;
            particle->timer = 0;
        }
        return 0;

    case 2:
        particle->timer++;
        if (func_800C6B90(&particle->position.x, 200) &&
            D_800E2368->flags &&
            (D_800F32D0->actor->state->core_flags & 0x3F000000)
                == 0x01000000) {
            D_8009D254->state->flags |= 0x4000;
            D_800F32D0->actor->state->core_flags =
                (D_800F32D0->actor->state->core_flags & 0xC0FFFFFF)
                | 0x29000000;
            D_800F32D0->actor->state->core_flags |= 0x80000000;
        }
        returnTest = particle->timer < 24;
        goto test_timer;

    case 3:
        returnTest = (unsigned short)particle->timer + 1;
        particle->timer = returnTest;

        returnTest = (s16)returnTest < 12;
test_timer:

        if (returnTest) return 0;
        return 1;

        }
        return 0;
    case 2:
        switch (particle->state) {
    case 1:
        intensity = 128;
        if (particle->timer & 1)
            intensity = 92;
        rotation.x = 0;
        rotation.y = 0;
        signedValue = particle->timer;
        rotation.z = signedValue << 7;
        sum = 64;
        D_800F3368.parameter00 = sum;
        D_800F3376 = sum;
        D_800F3378 = sum;
        rotation.pad = 0;
        D_800F336A = 4;
        palette = D_800E1204[D_800F336C];
        if (D_800F336C == 4 && D_800F3428)
            palette += 4;
        clut = func_80077AA4(16, palette);
        func_800CEE20(&particle->position, &rotation, 6144, 6144,
                     68, clut, 1, intensity / 2, 0);
        D_800F336A = 2;
        asm volatile("" : : : "memory");
        D_800F3368.parameter00 = 32;
        D_800F3376 = 32;
        D_800F3378 = 32;
        negValue = rotation.z;
        rotation.z = -negValue;
        palette = D_800E1204[D_800F336C];
        if (D_800F336C == 4 && D_800F3428)
            palette += 4;
        clut = func_80077AA4(16, palette);
        func_800CEE20(&particle->position, &rotation, 8192, 8192,
                     152, clut, 1, intensity, 0);
        gte_ldrotmatrix(D_800BCFA4.value);
        gte_ldtransmatrix(D_800BCFA4.value);
        func_800D004C(&particle->position, 600, 600, 5,
                     0, 4096, 4096, &color, 0, 70, 1);
        if (particle->timer < 24 && (particle->position.pad & 1)) {
            intensity = func_80077DC4((particle->timer << 10) / 24) / 64;
floorIndex = D_800F336C;
temporaryPosition.x = particle->position.x;
temporaryPosition.y = particle->position.y;
pulseSum = 64;
floorZ = particle->position.z;
asm volatile("" : : : "memory");

temporaryPosition.z = floorZ;
D_800F336A = 4;
D_800F3368.parameter00 = pulseSum;
D_800F3376 = pulseSum;
D_800F3378 = pulseSum;
palette = D_800E1204[floorIndex];
if (floorIndex == 4 && D_800F3428) palette += 4;
            clut = func_80077AA4(160, palette);
            func_800D3BC8(&temporaryPosition, 6144, 6144, 12,
                         clut, 1, intensity / 2, 0, 3072);
            D_800F3368.parameter00 = 32;
            D_800F336A = 2;
            D_800F3376 = 32;
            D_800F3378 = 32;
            palette = D_800E1204[D_800F336C];
            if (D_800F336C == 4 && D_800F3428)
                palette += 4;
            clut = func_80077AA4(176, palette);
            func_800D3BC8(&temporaryPosition, 6144, 6144, 154,
                         clut, 1, intensity / 2, 0, 1536);
        }
        gte_ldrotmatrix(D_800BCFA4.value);
        gte_ldtransmatrix(D_800BCFA4.value);
        if (particle->timer < 13 && !(particle->position.pad & 1)) {
            func_800D0E88(&particle->position, &particle->angles,
                         1900, (12 - particle->timer) << 8,
                         &color, 0, 0, 62, 1);
        }
        break;
    case 2:
        divider = 0x2aaa0000;
asm("" : : "r"(divider));
        D_800F3368.parameter00 = 64;
        D_800F336A = 4;
        D_800F3376 = 64;
        D_800F3378 = 64;
        asm volatile("" : : : "memory");
        intensity = func_80077DC4((particle->timer << 10) / 24) / 32;
        scale = func_80077DC4((particle->timer << 10) / 24) * 2;
        if (particle->timer & 1)
            intensity = intensity * 2 / 3;
        palette = D_800E1204[D_800F336C];
        if (D_800F336C == 4 && D_800F3428)
            palette += 4;
        clut = func_80077AA4(16, palette);
        func_800CEE20(&particle->position, 0, scale, scale,
                     8, clut, 1, intensity, 0);
        floorIndex = D_800F336C;
        floor_rotation.x = 1024;
        floor_rotation.y = 0;
        floorTimer = particle->timer;
        floor_rotation.z = floorTimer << 5;
        floor_rotation.flags = 1;
        floor_position.x = particle->position.x;
        floorY = D_800942EC;
        sum = (u16)particle->position.z;

        floor_position.y = floorY;
        floor_position.z = sum;
        palette = D_800E1204[floorIndex];
        if (floorIndex == 4 && D_800F3428)
            palette += 4;
        clut = func_80077AA4(16, palette);
        func_800CEE20(&floor_position, &floor_rotation,
                     8192 - scale, 8192 - scale,
                     64, clut, 1, intensity / 2, 0);
        D_800F3368.parameter00 = 32;
        D_800F336A = 2;
        D_800F3376 = 32;
        D_800F3378 = 32;
        break;

        }
        return 0;
    }
    return 0;
timer_return_zero:
    return 0;
timer_return_one:
    return 1;
}
