#include "../room_lib/room_lib.h"

extern void RoomLib_RearmOnMatch_80192258(void);

ROOMLIB_INIT_TIMER_ARGS(func_80192190, RoomLib_RearmOnMatch_80192258, 0x08064888)

int func_80192228(void) {
    return 0;
}
