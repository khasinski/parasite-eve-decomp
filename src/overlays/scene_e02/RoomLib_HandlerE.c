/* MASPSX_FLAGS: --expand-div */
#include "../room_lib/room_lib.h"

int RoomLib_ResetAndSignalB_80191EA4(RoomEnt *o);

#define SCENE_E02_STEER(link, state) RoomLib_FxNotify2(link, (struct RoomSub *)(state))
ROOMLIB_HANDLER_E(RoomLib_HandlerE, RoomLib_ResetAndSignalB_80191EA4,
                  SCENE_E02_STEER, D_800966EC)
