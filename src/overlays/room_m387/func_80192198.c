#include "../room_lib/room_lib.h"

extern void RoomLib_RearmOnMatch_80192260(void);

ROOMLIB_INIT_TIMER_ARGS(func_80192198, RoomLib_RearmOnMatch_80192260, 0x0806488A)

int func_80192230(void) {
    return 0;
}
