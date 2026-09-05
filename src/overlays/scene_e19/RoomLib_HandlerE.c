/* MASPSX_FLAGS: --expand-div */
#include "../room_lib/room_lib.h"

int RoomLib_ResetAndSignalB_80190A24(RoomEnt *o);

#define SCENE_E19_STEER(link, state) RoomLib_FxNotify2(link, (struct RoomSub *)(state))
ROOMLIB_HANDLER_E(RoomLib_HandlerE, RoomLib_ResetAndSignalB_80190A24,
                  SCENE_E19_STEER, D_800966EC)
