#include "../room_lib/room_lib.h"

extern void RoomLib_RearmOnMatch_80192200(void);

ROOMLIB_INIT_TIMER_ARGS(func_80192138, RoomLib_RearmOnMatch_80192200, 0x08064872)
