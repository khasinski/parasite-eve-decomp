#include "room_m273.h"

int func_801947CC(int mode, RoomM273PaletteInput *input)
{
    RoomM273TrigEntry *entry;
    RoomM273PaletteEffect *effect;
    GteShortVector position;
    GteVector *source;
    int x;
    int y;
    int z;
    GteRotation rotation;
    int random;

    if (mode == 1) {
        int frame;
        frame = D_800E27EC;
        if (frame >= 33) return 1;
        entry = &D_800966EC[(((unsigned int)frame << 7) & 0x3F80) / sizeof(RoomM273TrigEntry)];
        input->size = (entry->low * 3 * 2048) / 4096 + 4096;
        if (frame >= 24) return 0;
        effect = func_800CE610(D_8019AE94);
        if (!effect) return 0;
        effect->source = input->source;
        effect->size = (input->size * 3 * 256) / 4096;
        effect->depth = Inv_ScrambleGrid() + 256;
        effect->color = D_8019AC30[D_800E27EC & 3];
        effect->x = Inv_ScrambleGrid() << 4;
        random = Inv_ScrambleGrid();
        effect->y = (128 - random) >> 2;
    } else if (mode == 2) {
        int frame;
        source = input->source;
        frame = D_800E27EC;
        y = source->y;
        x = source->x;
        frame = frame - 1;
        position.y = y;
        z = source->z;
        entry = &D_800966EC[(((unsigned int)frame << 7) & 0x3F80) / sizeof(RoomM273TrigEntry)];
        position.z = z;
        position.x = x;
        rotation.x = 0;
        rotation.y = 0;
        rotation.z = 170 * frame;
        rotation.flags = 0;
        func_800D004C(&position, 256, 256, 12, &rotation,
                       input->size, input->size,
                       (RenderColor *)D_8019AB70,
                       (RenderColor *)&D_8019ABFC[D_8019AE98],
                       entry->high >> 5, 1);
    }
    return 0;
}
