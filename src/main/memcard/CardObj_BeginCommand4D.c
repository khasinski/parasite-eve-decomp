/* MASPSX_FLAGS: --stack-return-delay */
/* CC1_FLAGS: -G8 */

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
