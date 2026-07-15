/* MASPSX_FLAGS: --stack-return-delay */
#include "pe1/card_obj.h"

extern CardObj *(*D_8009B738)(void);

void CardObj_BeginReadIdCommand(CardObj *obj, int byte1, int byte2);

void CardObj_StartCommandWithBytes(int channel, unsigned char byte1, unsigned char byte2) {
    CardObj_BeginReadIdCommand(D_8009B738(), byte1, byte2);
}
