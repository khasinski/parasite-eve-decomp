#include "../room_lib/room_lib.h"

/* This room's variant returns void; room_m256 defines the same address as an
 * int dispatcher, so the shared header cannot declare it for both. */
extern void func_801902D4(RoomEnt *obj);

ROOMLIB_HANDLER_E_ARGS(RoomLib_HandlerEArgs_8018FFCC,
                       func_801902D4)
