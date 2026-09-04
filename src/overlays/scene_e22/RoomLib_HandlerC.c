/* MASPSX_FLAGS: --expand-div */
#define ROOMLIB_HANDLER_C_PHASE func_80191D60
#define ROOMLIB_HANDLER_C_RESET func_801920F0
#define ROOMLIB_HANDLER_C_GTE_LOAD_VECTOR(base) \
    asm volatile("addiu $2,%0,0x20\n\t" \
                 "lwc2 $0,0($2)\n\t" \
                 "lwc2 $1,4($2)" \
                 : : "r"(base) : "$2", "memory")
#define ROOMLIB_HANDLER_C_GTE_STORE_MAC(base) \
    asm volatile("addiu $2,%0,0x38\n\t" \
                 "swc2 $25,0($2)\n\t" \
                 "swc2 $26,4($2)\n\t" \
                 "swc2 $27,8($2)" \
                 : : "r"(base) : "$2", "memory")
#include "../room_lib/RoomLib_HandlerC.inc"
