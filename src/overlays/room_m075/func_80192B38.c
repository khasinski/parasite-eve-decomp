/* MASPSX_FLAGS: --expand-div */
#define func_80192B38 func_80192B38_header_decl
#define RoomLib_ResetAndSignal_80192ED8 RoomLib_ResetAndSignal_80192ED8_header_decl
#include "../room_lib/room_lib.h"
#undef RoomLib_ResetAndSignal_80192ED8
#undef func_80192B38

extern int RoomLib_ResetAndSignal_80192ED8(RoomEnt *obj);

#define ROOMLIB_ADVANCE_ARC_NAME func_80192B38
#define ROOMLIB_ADVANCE_ARC_RESET RoomLib_ResetAndSignal_80192ED8
#include "../room_lib/RoomLib_AdvanceArcToTarget.inc"
