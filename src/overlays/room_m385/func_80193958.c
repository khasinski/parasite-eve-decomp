#define func_80193958 func_80193958_header_decl
#define RoomLib_ResetAndSignal_80193CE0 RoomLib_ResetAndSignal_80193CE0_header_decl
#include "../room_lib/room_lib.h"
#undef RoomLib_ResetAndSignal_80193CE0
#undef func_80193958

extern int RoomLib_ResetAndSignal_80193CE0(RoomEnt *obj);

#define ROOMLIB_ADVANCE_ARC_Y_NAME func_80193958
#define ROOMLIB_ADVANCE_ARC_Y_RESET RoomLib_ResetAndSignal_80193CE0
#include "../room_lib/RoomLib_AdvanceArcToTargetY.inc"
