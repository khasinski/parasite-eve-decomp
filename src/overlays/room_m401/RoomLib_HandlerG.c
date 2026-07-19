#include "../room_lib/room_lib.h"

extern void func_80194A54(void);
extern int RoomLib_Set4ClearSignal_80194E14(void *);

ROOMLIB_HANDLER_G(RoomLib_HandlerG, func_80194A54, RoomLib_Set4ClearSignal_80194E14)
