/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: --stack-return-delay */

#include "pe1/card_obj.h"

extern int (*g_MemCardIsTransferActiveFn)(void *);

void CardObj_EmitCommand4D(void *);
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

void CardObj_EmitCommand4D(void *arg0)
{
    int value = *(int *)((char *)arg0 + 0x20);

    *((unsigned char *)arg0 + 0x36) = 0x4D;
    *((unsigned char *)arg0 + 0x35) = 6;
    *(int *)((char *)arg0 + 0x2C) = value;
}
