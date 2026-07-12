/* CC1_FLAGS: -fno-schedule-insns */
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
        register unsigned char *obj_s0 asm("$16") = (unsigned char *)obj;
        register int one_v1 asm("$3");
        register int card_id_a0 asm("$4");

        ret = 1;
        asm volatile("" : "=r"(ret) : "0"(ret));
        card_id_a0 = obj_s0[0xE4];
        asm volatile("" : "=r"(card_id_a0) : "0"(card_id_a0));
        one_v1 = 1;
        obj_s0[0x46] = one_v1;
        one_v1 = (int)func_80083D9C;
        *(int *)(obj_s0 + 0x14) = one_v1;
        one_v1 = (int)func_80083DF0;
        *(int *)(obj_s0 + 0x18) = one_v1;
        one_v1 = arg1_copy & 0xFF;
        obj_s0[0x51] = arg1;
        obj_s0[0x52] = arg2;
        asm volatile("" ::: "memory");
        one_v1 = one_v1 ^ card_id_a0;
        one_v1 = one_v1 < 1U;
        obj_s0[0x53] = one_v1;
        return ret;
    }
}
