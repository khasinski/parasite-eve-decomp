#define func_80192D0C func_80192D0C_header_decl
#define RoomLib_ResetAndSignal_80193094 RoomLib_ResetAndSignal_80193094_header_decl
#include "../room_lib/room_lib.h"
#undef RoomLib_ResetAndSignal_80193094
#undef func_80192D0C

extern int RoomLib_ResetAndSignal_80193094(RoomEnt *obj);

#define ROOMLIB_ADVANCE_ARC_Y_NAME func_80192D0C
#define ROOMLIB_ADVANCE_ARC_Y_RESET RoomLib_ResetAndSignal_80193094
#include "../room_lib/RoomLib_AdvanceArcToTargetY.inc"
