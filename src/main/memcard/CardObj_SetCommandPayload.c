/* GCC_VERSION: 2.8.1 */
#include "pe1/card_obj.h"

extern CardObj *(*D_8009B738)(void);

void func_800835A4(CardObj *obj, int payload, int size);

void CardObj_SetCommandPayload(int channel, int payload, int size) {
    func_800835A4(D_8009B738(), payload, size);
}
