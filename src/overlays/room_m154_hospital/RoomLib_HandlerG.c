#include "../room_lib/room_lib.h"

extern void func_80193E10(void);
extern int RoomLib_Set4ClearSignal_801941D0(void *);

ROOMLIB_HANDLER_G(RoomLib_HandlerG, func_80193E10, RoomLib_Set4ClearSignal_801941D0)
