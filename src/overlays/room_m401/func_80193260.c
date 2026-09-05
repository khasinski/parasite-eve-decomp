/* MASPSX_FLAGS: --expand-div */
#define func_80193260 func_80193260_header_decl
#define RoomLib_ResetAndSignal_80193600 RoomLib_ResetAndSignal_80193600_header_decl
#include "../room_lib/room_lib.h"
#undef RoomLib_ResetAndSignal_80193600
#undef func_80193260

extern int RoomLib_ResetAndSignal_80193600(RoomEnt *obj);

#define ROOMLIB_ADVANCE_ARC_NAME func_80193260
#define ROOMLIB_ADVANCE_ARC_RESET RoomLib_ResetAndSignal_80193600
#include "../room_lib/RoomLib_AdvanceArcToTarget.inc"
