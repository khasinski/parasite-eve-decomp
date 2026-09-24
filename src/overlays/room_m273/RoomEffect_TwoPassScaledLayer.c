#include "room_m273.h"

int func_80194B5C(int mode, GteShortVector *input)
{
    GteRotation rotation;
    GteShortVector position;
    int frame;
    RoomM273TrigEntry *entry;
    int size;
    s16 *indexPtr;
    RenderColor *sprite;

    if (mode == 1) {
        if (D_800E27EC >= 16) return 1;
    } else if (mode == 2) {
        frame = (s16)(D_800E27EC - 1);
        entry = &D_800966EC[(((unsigned int)frame << 8) & 0x3F00) / sizeof(RoomM273TrigEntry)];
        size = (entry->low * 5 * 2048) / 4096 + 2048;
        position = *input;
        indexPtr = &D_8019AE98;
        sprite = (RenderColor *)D_8019AB70;
        rotation.x = 0;
        rotation.y = 0;
        rotation.z = frame * 204;
        rotation.flags = 0;
        func_800D004C(&position, 160, 160, 10, &rotation, (s16)size, (s16)size,
                       sprite, (RenderColor *)&D_8019ABFC[*indexPtr], entry->high >> 5, 1);

        position.y = D_800942EC;
        rotation.x = 1024;
        rotation.flags = 1;
        func_800D004C(&position, 256, 256, 10, &rotation,
                       (s16)size >> 1, (s16)size >> 1,
                       (RenderColor *)&D_8019ABFC[*indexPtr], sprite, entry->high >> 5, 1);
    }
    return 0;
}
