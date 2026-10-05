/* MASPSX_FLAGS: --expand-div */
#define func_80192D50 func_80192D50_header_decl
#define RoomLib_ResetAndSignal_801930F0 RoomLib_ResetAndSignal_801930F0_header_decl
#include "../room_lib/room_lib.h"
#undef RoomLib_ResetAndSignal_801930F0
#undef func_80192D50

extern int RoomLib_ResetAndSignal_801930F0(RoomEnt *obj);

#define ROOMLIB_ADVANCE_ARC_NAME func_80192D50
#define ROOMLIB_ADVANCE_ARC_RESET RoomLib_ResetAndSignal_801930F0
#include "../room_lib/RoomLib_AdvanceArcToTarget.inc"
