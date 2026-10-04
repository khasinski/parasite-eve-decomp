#include "pe1/scene_particle_slot_draw.h"
#include "scene_particle_slots.h"

/* Draw each slot's sprite at its offset from the scene anchor (doubled in
 * depth), then each slot's shadow sprite using the raw offset. */
void func_80191E78(void *object, void *timer, SceneParticleSlots *slots)
{
    GteShortVector position;
    GteShortVector offset;
    RenderColor color;
    int scale;
    int kind;
    int palette;
    u32 i;

    func_80071A44(&offset, 0, 8);
    offset.x = D_8019956C;
    offset.y = 0;
    offset.z = D_8019957C;
    position = offset;
    offset = D_8018F040;
    color = D_8018F048;
    D_800F3368.tpage = D_800E2850[D_800E11EA];
    D_800F3368.palette = 3;
    D_800F3368.parameter06 = 0;
    D_800F3368.parameter00 = 0x40;
    D_800F3368.parameter02 = 4;
    D_800F3368.extent_x = 0x40;
    D_800F3368.extent_y = 0x40;
    D_800F3368.depth = 1000;
    D_800F3368.parameter0A = 0;
    for (i = 0; i < 4; i++) {
        offset = slots->offset[i];
        color.b = 0xFF;
        color.g = 0xFF;
        color.r = 0xFF;
        offset.z <<= 1;
        scale = slots->phase[i] * slots->motionRamp >> 12;
        kind = D_800F3368.palette;
        palette = D_800E1204[kind];
        if (kind == 4 && D_800F3428 != 0) {
            palette += 4;
        }
        func_800CEE20(&position, (GteRotation *)&offset, scale, scale, 0,
                      (u16)func_80077AA4(0, palette), 1, slots->verticalOffset[i], 0);
    }
    D_800F3368.parameter00 = 0x40;
    D_800F3368.parameter02 = 4;
    D_800F3368.extent_x = 0x40;
    D_800F3368.extent_y = 0x40;
    D_800F3368.depth = 0;
    D_800F3368.parameter0A = 5;
    for (i = 0; i < 4; i++) {
        u16 *palettes = D_800E1204;
        int special = 4;

        scale = slots->phase[i] * slots->motionRamp >> 12;
        kind = D_800F3368.palette;
        palette = palettes[kind];
        if (kind == special && D_800F3428 != 0) {
            palette += 4;
        }
        func_800CEE20(&position, (GteRotation *)&slots->offset[i], scale, scale, 0x80,
                      (u16)func_80077AA4(0x80, palette), 2, slots->verticalOffset[i], 0);
    }
}
