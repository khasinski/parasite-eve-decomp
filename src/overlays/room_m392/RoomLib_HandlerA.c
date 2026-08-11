#include "../room_lib/room_lib.h"

extern char *D_8009D254;
extern int D_800966EC[];
extern void RoomLib_MoveActorLocal_801934AC(void);

ROOMLIB_FACE_ACTOR_WITH_GLOBALS(RoomLib_HandlerA, RoomLib_MoveActorLocal_801934AC, D_8009D254, D_800966EC)
