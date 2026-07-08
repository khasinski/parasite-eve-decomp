/* MASPSX_FLAGS: --stack-return-delay */

#include "pe1/card_obj.h"

extern int (*g_MemCardIsTransferActiveFn)(void *);

void CardObj_EmitCommand4D(CardObj *);
void Render_CheckParticleBounds(void *);

int CardObj_BeginCommand4D(CardObj *obj, int arg1) {
    if (g_MemCardIsTransferActiveFn(obj) != 0) {
        return 0;
    }

    obj->field_46 = 1;
    obj->fn_14 = CardObj_EmitCommand4D;
    obj->field_20 = arg1;
    obj->fn_18 = Render_CheckParticleBounds;
    return 1;
}

void CardObj_EmitCommand4D(CardObj *arg0)
{
    int value = arg0->field_20;

    arg0->command = 0x4D;
    arg0->payload_2c_len = 6;
    arg0->payload_2c = (unsigned char *)value;
}
