/* MASPSX_FLAGS: --stack-return-delay */

#include "pe1/card_obj.h"

typedef int (*Callback0)(void);
typedef int (*Callback1)(void *);

extern int D_8009B764;
extern int D_8009B774;
extern int D_8009B760;
extern int D_8009B7A4;
extern Callback0 D_8009B754;
extern Callback0 D_8009B750;
extern Callback1 D_8009B73C;

int MemCard_WriteByte(CardObj *obj, int value);

int CardObj_WriteCommandByte(CardObj *obj) {
    register CardObj *call_obj asm("$4");
    register int cmd asm("$2");
    register int value asm("$5");

    if (D_8009B764 == D_8009B774 && D_8009B760 != 0) {
        D_8009B754();
        D_8009B750();
    }

    if (D_8009B7A4 != 0) {
        D_8009B73C(obj->field_0c);
        D_8009B73C(obj->field_0c + 0xF0);
    }

    call_obj = obj;
    cmd = obj->command;
    if (cmd != 0) {
        goto use_command;
    }
    value = 0x42;
    goto write;

use_command:
    value = *(volatile unsigned char *)&obj->command;

write:
    return MemCard_WriteByte(call_obj, value);
}
