#include "room_m273.h"

typedef struct {
    u16 *source;
} RoomM273PhaseState;

int func_8019A290(int mode, RoomM273PhaseState *state) {
    RoomM273Vector position;
    int frame, size;
    RoomM273TrigEntry *entry;

    if (mode == 1) {
        if (D_800E27EC >= 16) return 1;
    } else if (mode == 2) {
        frame = D_800E27EC - 1;
        entry = &D_800966EC[((frame << 8) & 0x3F00) >> 2];
        size = (s16)(entry->low * 2 + 2048);
        position.x = state->source[0];
        position.y = D_800942EC;
        position.z = state->source[2];
        func_800D004C(&position, 192, 192, 10,
                       D_8019ACCC + ((frame & 1) << 3), size, size,
                       D_8019AB70, D_8019ACC8, entry->high >> 5, 1);
    }
    return 0;
}
