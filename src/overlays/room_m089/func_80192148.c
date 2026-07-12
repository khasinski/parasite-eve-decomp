#include "../room_lib/room_lib.h"

extern void RoomLib_RearmOnMatch_80192210(void);

ROOMLIB_INIT_TIMER_ARGS(func_80192148, RoomLib_RearmOnMatch_80192210, 0x08064876)
