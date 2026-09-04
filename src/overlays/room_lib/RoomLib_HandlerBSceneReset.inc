extern char *g_PlayerEntity;

#define ROOMLIB_HANDLER_B_ACTOR_PTR g_PlayerEntity
#define ROOMLIB_HANDLER_B_RESET_ON_ARM
#define ROOMLIB_HANDLER_B_GTE_LOAD_VECTOR(base) \
    asm volatile("addiu $2,%0,0x20\n\t" \
                 "lwc2 $0,0($2)\n\t" \
                 "lwc2 $1,4($2)" \
                 : : "r"(base) : "$2", "memory")
#define ROOMLIB_HANDLER_B_GTE_STORE_MAC(base) \
    asm volatile("addiu $2,%0,0x28\n\t" \
                 "swc2 $25,0($2)\n\t" \
                 "swc2 $26,4($2)\n\t" \
                 "swc2 $27,8($2)" \
                 : : "r"(base) : "$2", "memory")
#include "RoomLib_HandlerB.inc"
