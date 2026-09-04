#define func_80192268 func_80192268_header_decl
#define RoomLib_ResetAndSignal_80192608 RoomLib_ResetAndSignal_80192608_header_decl
#include "../room_lib/room_lib.h"
#undef RoomLib_ResetAndSignal_80192608
#undef func_80192268

extern int RoomLib_ResetAndSignal_80192608(RoomEnt *obj);

#define ROOMLIB_ADVANCE_ARC_NAME func_80192268
#define ROOMLIB_ADVANCE_ARC_RESET RoomLib_ResetAndSignal_80192608
#include "../room_lib/RoomLib_AdvanceArcToTarget.inc"
