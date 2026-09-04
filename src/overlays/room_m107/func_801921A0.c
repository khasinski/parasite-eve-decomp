#define func_801921A0 func_801921A0_header_decl
#define RoomLib_ResetAndSignal_80192540 RoomLib_ResetAndSignal_80192540_header_decl
#include "../room_lib/room_lib.h"
#undef RoomLib_ResetAndSignal_80192540
#undef func_801921A0

extern int RoomLib_ResetAndSignal_80192540(RoomEnt *obj);

#define ROOMLIB_ADVANCE_ARC_NAME func_801921A0
#define ROOMLIB_ADVANCE_ARC_RESET RoomLib_ResetAndSignal_80192540
#include "../room_lib/RoomLib_AdvanceArcToTarget.inc"
