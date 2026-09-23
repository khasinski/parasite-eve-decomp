#define RoomMain_ActorPtr2 g_PlayerEntity
#include "../room_lib/room_lib.h"

extern RoomBlob8 D_8018F220;
extern int func_80193F64(int mode, RoomDlgAnimState *state, RoomDlgAnimParams *params);

ROOMLIB_MSG_DISPATCH(func_80194924, D_8018F220, func_80193F64)
