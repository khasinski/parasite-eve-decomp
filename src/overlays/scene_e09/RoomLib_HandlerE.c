/* MASPSX_FLAGS: --expand-div */
#include "../room_lib/room_lib.h"

int RoomLib_ResetAndSignalB_801923FC(RoomEnt *o);

#define SCENE_E09_STEER(link, state) RoomLib_FxNotify2(link, (struct RoomSub *)(state))
ROOMLIB_HANDLER_E(RoomLib_HandlerE, RoomLib_ResetAndSignalB_801923FC,
                  SCENE_E09_STEER, D_800966EC)
