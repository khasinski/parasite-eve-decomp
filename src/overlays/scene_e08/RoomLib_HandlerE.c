/* MASPSX_FLAGS: --expand-div */
#include "../room_lib/room_lib.h"

int RoomLib_ResetAndSignalB_80196B0C(RoomEnt *o);

#define SCENE_E08_STEER(link, state) RoomLib_FxNotify2(link, (struct RoomSub *)(state))
ROOMLIB_HANDLER_E(RoomLib_HandlerE, RoomLib_ResetAndSignalB_80196B0C,
                  SCENE_E08_STEER, D_800966EC)
