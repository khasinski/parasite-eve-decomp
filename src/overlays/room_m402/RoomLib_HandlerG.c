#include "../room_lib/room_lib.h"

extern void func_80193900(void);
extern void RoomLib_Set4ClearSignal_80193CC0(void *);

ROOMLIB_HANDLER_G(RoomLib_HandlerG, func_80193900, RoomLib_Set4ClearSignal_80193CC0)
