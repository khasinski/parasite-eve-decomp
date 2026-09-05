/* MASPSX_FLAGS: --expand-div */
#define func_80195894 func_80195894_header_decl
#define RoomLib_ResetAndSignal_80195C34 RoomLib_ResetAndSignal_80195C34_header_decl
#include "../room_lib/room_lib.h"
#undef RoomLib_ResetAndSignal_80195C34
#undef func_80195894

extern int RoomLib_ResetAndSignal_80195C34(RoomEnt *obj);

#define ROOMLIB_ADVANCE_ARC_NAME func_80195894
#define ROOMLIB_ADVANCE_ARC_RESET RoomLib_ResetAndSignal_80195C34
#include "../room_lib/RoomLib_AdvanceArcToTarget.inc"
