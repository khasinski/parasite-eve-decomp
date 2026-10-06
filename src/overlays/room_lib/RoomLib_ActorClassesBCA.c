/* MASPSX_FLAGS: --expand-div */
/*
 * Room library, second object: the actor classes driven by RoomLib_HandlerB,
 * RoomLib_HandlerC and RoomLib_HandlerA (see RoomLib_ActorClassesDE.c for
 * the library and the object boundary).
 */
#include "room_lib.h"

ROOMLIB_RETURN_ZERO(RoomLib_HandlerBNop0)
ROOMLIB_INIT_C(RoomLib_InitHandlerB, RoomLib_HandlerB)
ROOMLIB_HANDLER_B_ARGS(RoomLib_ConfigureHandlerB, RoomLib_ArmHandlerB,
                       RoomLib_AdvanceArcToTarget)
ROOMLIB_RETURN_ZERO(RoomLib_HandlerBNop3)
ROOMLIB_INVOKE_CALLBACK_C(RoomLib_UpdateHandlerB)
ROOMLIB_ARM_IF_WINDOW_A(RoomLib_ArmHandlerB, RoomLib_HandlerB)

#define ROOMLIB_HANDLER_B_ACTOR_PTR D_8009D254
#define ROOMLIB_HANDLER_B_ROT_TABLE D_800966EC
#define ROOMLIB_HANDLER_B_PHASE RoomLib_HandlerBPhase
#define ROOMLIB_HANDLER_B_RESET RoomLib_ReleaseHandlerB
#include "RoomLib_HandlerB.inc"

ROOMLIB_RETURN_VOID_ENT(RoomLib_HandlerBPhase)

#define ROOMLIB_ADVANCE_ARC_NAME RoomLib_AdvanceArcToTarget
#define ROOMLIB_ADVANCE_ARC_RESET RoomLib_ReleaseHandlerB
#include "RoomLib_AdvanceArcToTarget.inc"

ROOMLIB_RESET_AND_SIGNAL(RoomLib_ReleaseHandlerB)
ROOMLIB_RETURN_ZERO(RoomLib_HandlerBNop6)

ROOMLIB_RETURN_ZERO(RoomLib_HandlerCNop0)
ROOMLIB_INIT_FULL(RoomLib_InitHandlerC, RoomLib_HandlerC)
ROOMLIB_HANDLER_C_ARGS(RoomLib_ConfigureHandlerC, RoomLib_ArmHandlerC,
                       RoomLib_AdvanceArcToTargetY)
ROOMLIB_RETURN_ZERO(RoomLib_HandlerCNop3)
ROOMLIB_INVOKE_CALLBACK_C(RoomLib_UpdateHandlerC)
ROOMLIB_ARM_IF_WINDOW_B(RoomLib_ArmHandlerC, RoomLib_HandlerC)

#define ROOMLIB_HANDLER_C_ACTOR_PTR D_8009D254
#define ROOMLIB_HANDLER_C_ROT_TABLE D_800966EC
#define ROOMLIB_HANDLER_C_PHASE RoomLib_HandlerCPhase
#define ROOMLIB_HANDLER_C_RESET RoomLib_ReleaseHandlerC
#include "RoomLib_HandlerC.inc"

ROOMLIB_RETURN_VOID(RoomLib_HandlerCPhase)

#define ROOMLIB_ADVANCE_ARC_Y_NAME RoomLib_AdvanceArcToTargetY
#define ROOMLIB_ADVANCE_ARC_Y_RESET RoomLib_ReleaseHandlerC
#include "RoomLib_AdvanceArcToTargetY.inc"

ROOMLIB_RESET_AND_SIGNAL(RoomLib_ReleaseHandlerC)
ROOMLIB_RETURN_ZERO(RoomLib_HandlerCNop6)

ROOMLIB_RETURN_ZERO(RoomLib_HandlerANop0)
ROOMLIB_INIT_TIMERS(RoomLib_InitHandlerA)
ROOMLIB_ARG_DISPATCH_REARM(RoomLib_ConfigureHandlerA, RoomLib_RearmHandlerA)
ROOMLIB_RETURN_ZERO(RoomLib_HandlerANop3)
ROOMLIB_INVOKE_CALLBACK_C(RoomLib_UpdateHandlerA)
ROOMLIB_REARM_ON_MATCH(RoomLib_RearmHandlerA)
ROOMLIB_FACE_ACTOR_WITH_GLOBALS(RoomLib_HandlerA, RoomLib_HandlerF, D_8009D254,
                                D_800966EC)
ROOMLIB_MOVE_ACTOR_LOCAL(RoomLib_HandlerF, RoomLib_ReleaseHandlerA)
ROOMLIB_SET4_CLEAR_SIGNAL(RoomLib_ReleaseHandlerA)
ROOMLIB_RETURN_ZERO(RoomLib_HandlerANop6)
