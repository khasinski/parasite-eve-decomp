#include "../room_lib/room_lib.h"

extern void func_801928FC(void);
extern void RoomLib_Set4ClearSignal_80192CBC(void *);

ROOMLIB_HANDLER_G(RoomLib_HandlerG, func_801928FC, RoomLib_Set4ClearSignal_80192CBC)
