#include "pe1/scene_e20_flare.h"
#include "pe1/render_object.h"
#include "pe1/gte.h"

extern RenderColor D_8018EFF4, D_8018EFF8;
extern u8 D_80190804[];
extern u16 D_800E11EA[];
int func_80077CF4(int angle);
int func_80077DC4(int angle);
u16 func_80077AA4(int x, int y);
void func_800D2104(GteShortVector *position, RenderColor *color, int size, int alpha);

/* Mode 1 ages the flare and, while it falls, moves and damps it and
 * bounces it off the floor; mode 2 draws a spinning glow with a streak,
 * a pulsing glow pair, or a fading spark. Matching debt: two register pins
 * retain the separate palette-compare temporaries. GTE transfers use the
 * shared instruction macros; there are no CPU instruction ASM bodies. */
int func_8018F028(int mode, SceneE20Particle *p)
{
    GteRotation rotation;
    RenderColor color;
    RenderColor streakColor;
    RenderColor glowColor;
    int fall;
    int bounce;
    int scale;
    int angle;
    int glow;
    int time;
    int state;

    streakColor = D_8018EFF4;
    glowColor = D_8018EFF8;
    switch (mode) {
    case 1:
        switch (p->kind) {
        case 0:
            p->timer++;
            if (p->timer < 0x10) break;
            return 1;
        case 1:
            p->timer++;
            if (p->timer < 0x10) break;
            return 1;
        case 2:
            p->timer++;
            p->position.x += p->velocity.x;
            p->position.y += p->velocity.y;
            p->position.z += p->velocity.z;
            p->velocity.x = p->velocity.x * 59 / 60;
            p->velocity.z = p->velocity.z * 59 / 60;
            fall = (u16)p->velocity.y + 1;
            p->velocity.y = fall;
            if (p->position.y >= D_800942EC.height) {
                bounce = -(s16)fall;
                p->velocity.y = bounce;
            }
            if (p->timer < 0x14) break;
            return 1;
        }
        break;
    case 2:
        state = p->kind;
        switch (state) {
        case 0: {
            int kind;
            int palette;
            D_800F3368.parameter00 = 0x20;
            D_800F3368.parameter02 = 2;
            D_800F3368.extent_x = 0x20;
            D_800F3368.extent_y = 0x20;
            {
                int tpage = D_800E2850[D_800E11EA[8]];
                D_800F3368.palette = 3;
                D_800F3368.parameter06 = 1;
                D_800F3368.tpage = tpage;
            }
            angle = p->timer << 6;
            scale = func_80077CF4(angle);
            rotation.x = p->velocity.x;
            rotation.y = p->velocity.y;
            rotation.z = p->velocity.z;
            rotation.flags = 1;
            {
                RenderMatrixSlot *matrixSlot = &D_800BCFA4;
                gte_ldrotmatrix(matrixSlot->value);
                gte_ldtransmatrix(matrixSlot->value);
            }
            func_800CF3AC(D_80190804, &color, p->timer);
            {
                register int specialKind asm("$3") = 4;
                kind = D_800F3368.palette;
                palette = D_800E1204[kind];
                if (kind == specialKind && D_800F3428 != 0) palette += 8;
                else palette += 4;
            }
            func_800CEE20(&p->position, &rotation, scale * 3, scale * 3, 4,
                          func_80077AA4(0, palette), 1, 0x80, &color);
            if (p->timer < 8) {
                register int streakKind asm("$3");
                int width = (p->timer / 2) << 5;
                int top = 0x40;
                glow = func_80077DC4(angle) / 32;
                func_800CF3AC(D_80190804, &color, p->timer << 1);
                streakKind = D_800F3368.palette;
                palette = D_800E1204[streakKind];
                if (streakKind == 4 && D_800F3428 != 0) palette += 7;
                else palette += 3;
                func_800D2370(&p->position, &rotation, 500, 0x168, width, top, 0x1F, 0x1F,
                              func_80077AA4(0, palette), &streakColor, &streakColor,
                              (s16)glow, 1);
            }
            break;
        }
        case 1: {
            int kind;
            int palette;
            time = p->timer;
            angle = time << 6;
            rotation.x = 0x400;
            rotation.y = 0;
            rotation.z = p->timer * 32;
            rotation.flags = 1;
            D_800F3368.parameter00 = 0x20;
            D_800F3368.parameter02 = 2;
            D_800F3368.extent_x = 0x20;
            D_800F3368.extent_y = 0x20;
            {
                int tpage = D_800E2850[D_800E11EA[0]];
                D_800F3368.palette = 3;
                D_800F3368.parameter06 = 0;
                D_800F3368.tpage = tpage;
            }
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            func_800CEE20(&p->position, 0, 0x2000, 0x2000,
                          (s16)D_800F3368.parameter02 * (p->timer / 2) + 0x60,
                          func_80077AA4(0, palette), state, 0x80, 0);
            scale = func_80077DC4(angle) / 2 + 0x800;
            glow = func_80077CF4(time << 7) / 32;
            {
                int tpage = D_800E2850[D_800E11EA[8]];
                D_800F3368.palette = 3;
                D_800F3368.parameter06 = 1;
                D_800F3368.tpage = tpage;
            }
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 9;
            else palette += 5;
            func_800CEE20(&p->position, 0, scale * 4, scale * 4, 6,
                          func_80077AA4(0, palette), 3, glow, &glowColor);
            break;
        }
        case 2:
            {
                RenderMatrixSlot *matrixSlot = &D_800BCFA4;
                gte_ldrotmatrix(matrixSlot->value);
                gte_ldtransmatrix(matrixSlot->value);
            }
            func_800CF3AC(D_80190804, &color, (p->timer << 4) / 20);
            func_800D2104(&p->position, &color, 0x80, 1);
            break;
        }
        break;
    }
    return 0;
}
