#include "../room_lib/room_lib.h"

extern void func_80192230(void);

#define RoomLib_HandlerA func_80192230
ROOMLIB_REARM_ON_MATCH(func_80192200)
