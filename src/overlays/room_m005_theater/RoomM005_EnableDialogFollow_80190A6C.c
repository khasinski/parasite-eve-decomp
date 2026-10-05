#include "../room_lib/room_lib.h"

struct RoomM005_Dialog;
extern struct RoomM005_Dialog RoomM005_DialogAnchor;

void *RoomM005_EnableDialogFollow_80190A6C(int mode) {
    if (mode == 1) {
        D_800E2368->h12 = 1;
    }
    return &RoomM005_DialogAnchor;
}
