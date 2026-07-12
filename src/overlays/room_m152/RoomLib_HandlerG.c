#include "../room_lib/room_lib.h"

extern void func_80192940(void);
extern void RoomLib_Set4ClearSignal_80192D00(void *);

ROOMLIB_HANDLER_G(RoomLib_HandlerG, func_80192940, RoomLib_Set4ClearSignal_80192D00)
