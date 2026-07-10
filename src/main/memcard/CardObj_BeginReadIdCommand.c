/* MASPSX_FLAGS: --stack-return-delay */
#include "pe1/card_obj.h"

extern int (*D_8009B740)(void *);

void func_80083D9C(CardObj *arg0);
int func_80083DF0(CardObj *arg0);

int CardObj_BeginReadIdCommand(CardObj *obj, int arg1, int arg2) {
    register int arg1_copy asm("$19") = arg1;

    if (D_8009B740(obj) != 0) {
        return 0;
    }

    {
        register int ret asm("$2");

        ret = 1;
        /* cc1psx schedules the byte compares too early in C; keep this success block local. */
        asm volatile(
            "lbu $4, 0xE4($16)\n"
            "addiu $3, $0, 1\n"
            "sb $3, 0x46($16)\n"
            "lui $3, %%hi(func_80083D9C)\n"
            "addiu $3, $3, %%lo(func_80083D9C)\n"
            "sw $3, 0x14($16)\n"
            "lui $3, %%hi(func_80083DF0)\n"
            "addiu $3, $3, %%lo(func_80083DF0)\n"
            "sw $3, 0x18($16)\n"
            "andi $3, $19, 0xFF\n"
            "sb $17, 0x51($16)\n"
            "sb $18, 0x52($16)\n"
            "xor $3, $3, $4\n"
            "sltiu $3, $3, 1\n"
            "sb $3, 0x53($16)"
            : "=r"(ret)
            : "0"(ret), "r"(obj), "r"(arg1), "r"(arg2), "r"(arg1_copy)
            : "$3", "$4", "memory");
        return ret;
    }
}
