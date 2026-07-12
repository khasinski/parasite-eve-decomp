#include "../room_lib/room_lib.h"

extern void RoomLib_RearmOnMatch_80192208(void);

ROOMLIB_INIT_TIMER_ARGS(func_80192140, RoomLib_RearmOnMatch_80192208, 0x08064874)
