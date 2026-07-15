/* MASPSX_FLAGS: --stack-return-delay */
#include "pe1/card_obj.h"

extern CardObj *(*D_8009B738)(void);

void func_80083BB8(CardObj *obj, int command);

void CardObj_StartCommand4D(int channel, int command) {
    func_80083BB8(D_8009B738(), command);
}
