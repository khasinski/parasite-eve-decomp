#include "../room_lib/room_lib.h"

extern char *D_8009D254;
extern int D_800966EC[];
extern void RoomLib_MoveActorLocal_80192350(void);

ROOMLIB_FACE_ACTOR_WITH_GLOBALS(RoomLib_HandlerA, RoomLib_MoveActorLocal_80192350, D_8009D254, D_800966EC)
