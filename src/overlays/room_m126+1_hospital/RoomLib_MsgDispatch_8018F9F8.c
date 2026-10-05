#include "../room_lib/room_lib.h"

extern RoomBlob8 RoomLib_DlgRect;
extern int RoomLib_DlgBlob(int mode, RoomDlgAnimState *state, RoomDlgAnimParams *params);

ROOMLIB_MSG_DISPATCH(RoomLib_MsgDispatch_8018F9F8, RoomLib_DlgRect, RoomLib_DlgBlob)
