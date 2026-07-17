#include "pe1/card_obj.h"

extern int (*D_8009B728)(void *);

int CardObj_CheckAbortOrDispatch(CardObj *arg0) {
    if (*((unsigned char *)arg0 + 0x53) != 0) {
        if (arg0->field_46 == 2) {
            return 1;
        }

        arg0->field_46 = 0xFE;
        return 0;
    }

    D_8009B728(arg0);
    return 0;
}
