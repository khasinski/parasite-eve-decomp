/* MASPSX_FLAGS: --expand-div */
/*
 * The room library: five actor classes, driven by RoomLib_HandlerD,
 * RoomLib_HandlerE, RoomLib_HandlerB, RoomLib_HandlerC and RoomLib_HandlerA.
 *
 * 117 room and scene overlays (and 8 more with a longer variant) link the
 * same 53 functions in the same order with the same four jump tables; this
 * unit is that library, compiled into each of them. Each class occupies the
 * seven-method run its room class table lists (no-op, Init, Configure,
 * no-op, Update, ..., Release, no-op), with the class's private states
 * between Update and Release. The zero word between the HandlerE and
 * HandlerB jump tables is the 8-byte alignment of the third table inside
 * this one object.
 */
#include "room_lib.h"
#include "handler_e_family.h"

ROOMLIB_RETURN_ZERO(RoomLib_HandlerDNop0)
ROOMLIB_INIT_B(RoomLib_InitHandlerD, RoomLib_HandlerD)
ROOMLIB_HANDLER_D_ARGS(RoomLib_ConfigureHandlerD, RoomLib_ArmHandlerD)
ROOMLIB_RETURN_ZERO(RoomLib_HandlerDNop3)
ROOMLIB_STATE_DISPATCH_VARIANT2(RoomLib_UpdateHandlerD, RoomLib_ReleaseHandlerD)
ROOMLIB_NOTIFY_AND_ARM_B(RoomLib_ArmHandlerD, RoomLib_HandlerD)

#define ROOMLIB_HANDLER_D_ROT_TABLE D_800966EC
#define ROOMLIB_HANDLER_D_TRANSFORM RoomLib_HandlerDTransformTarget
#define ROOMLIB_HANDLER_D_RESET RoomLib_ReleaseHandlerD
#include "RoomLib_HandlerD.inc"

ROOMLIB_FX_NOTIFY(RoomLib_FxNotify)
ROOMLIB_ROTATE_MOTION(RoomLib_HandlerDTransformTarget)
ROOMLIB_RESET_SIGNAL_WITH_TARGET_GATE(RoomLib_ReleaseHandlerD)
ROOMLIB_RETURN_ZERO(RoomLib_HandlerDNop6)

ROOMLIB_RETURN_ZERO(RoomLib_HandlerENop0)
ROOMLIB_INIT_D(RoomLib_InitHandlerE, RoomLib_HandlerE)
ROOMLIB_HANDLER_E_ARGS(RoomLib_ConfigureHandlerE, RoomLib_ArmHandlerE)
ROOMLIB_RETURN_ZERO(RoomLib_HandlerENop3)
ROOMLIB_STATE_DISPATCH_VARIANT2(RoomLib_UpdateHandlerE, RoomLib_ReleaseHandlerE)
ROOMLIB_NOTIFY2_AND_ARM_B_VIA(RoomLib_ArmHandlerE, RoomLib_HandlerE,
                              RoomLib_HandlerESteerToward)
ROOMLIB_HANDLER_E(RoomLib_HandlerE, RoomLib_ReleaseHandlerE,
                  RoomLib_HandlerESteerToward, D_800966EC)
ROOMLIB_STEER_TOWARD(RoomLib_HandlerESteerToward)
ROOMLIB_RESET_AND_SIGNAL_B(RoomLib_ReleaseHandlerE)
ROOMLIB_RETURN_ZERO(RoomLib_HandlerENop6)

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
