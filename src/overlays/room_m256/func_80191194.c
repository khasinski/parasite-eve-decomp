#define func_80191194 func_80191194_header_decl
#define RoomLib_ResetAndSignal_80191534 RoomLib_ResetAndSignal_80191534_header_decl
#include "../room_lib/room_lib.h"
#undef RoomLib_ResetAndSignal_80191534
#undef func_80191194

extern int RoomLib_ResetAndSignal_80191534(RoomEnt *obj);

#define ROOMLIB_ADVANCE_ARC_NAME func_80191194
#define ROOMLIB_ADVANCE_ARC_RESET RoomLib_ResetAndSignal_80191534
#include "../room_lib/RoomLib_AdvanceArcToTarget.inc"
