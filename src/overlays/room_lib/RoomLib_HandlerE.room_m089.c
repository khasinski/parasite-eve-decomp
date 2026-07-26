/* MASPSX_FLAGS: --expand-div */
#include "room_lib.h"

extern short D_800966EC[][2];
extern int RoomLib_ResetAndSignalB_80190990(RoomEnt *o);
extern void RoomLib_SteerToward_8019090C(RoomLink *link, RoomLibHandlerEState *state);

ROOMLIB_HANDLER_E(RoomLib_HandlerE, RoomLib_ResetAndSignalB_80190990,
                  RoomLib_SteerToward_8019090C, D_800966EC)
