#include "../room_lib/room_lib.h"
#define RoomMain_ActorPtr2 D_8009D254

extern RoomBlob8 D_8018F048;
extern void RoomLib_SetArgs3_80190D40(void);

#define SCENE_DIALOG_BLOB (*(RoomBlob8 *)((u8 *)RoomLib_SetArgs3_80190D40 + 0x1C))

ROOMLIB_MSG_DISPATCH(func_80191724, D_8018F048, SCENE_DIALOG_BLOB)
