/* PSY-Q LIBPAD PADCMD, part 1 of 7: _padSetAct, _padSetCmd, _padSendAtLoadInfo. */
#include "pe1/card_obj.h"

void CardObj_EmitCommand45(CardObj *arg0);
void CardObj_EmitCommand4C(CardObj *arg0, unsigned char arg1);
void CardObj_EmitCommand47(CardObj *arg0, unsigned char arg1);

void _padSetAct(CardObj *arg0, int arg1, unsigned char arg2)
{
    arg0->payload_28 = (unsigned char *)arg1;
    arg0->payload_28_len = arg2;
}

void _padSetCmd(CardObj *arg0, unsigned char arg1, int arg2, unsigned char arg3)
{
    arg0->command = arg1;
    arg0->payload_2c = (unsigned char *)arg2;
    arg0->payload_2c_len = arg3;
}

void _padSendAtLoadInfo(CardObj *arg0)
{
    int state = arg0->field_46;

    switch (state) {
    case 2:
        CardObj_EmitCommand45(arg0);
        break;
    case 3:
        CardObj_EmitCommand4C(arg0, arg0->field_e4);
        break;
    case 4:
        CardObj_EmitCommand47(arg0, arg0->field_47);
        break;
    }
}
