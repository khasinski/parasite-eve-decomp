#include "pe1/card_obj.h"

void CardObj_EmitCommand4C(CardObj *arg0, unsigned char arg1);
void CardObj_EmitCommand46(CardObj *arg0, unsigned char arg1);
void CardObj_EmitCommand47(CardObj *arg0, unsigned char arg1);
void CardObj_EmitCommand4B(CardObj *arg0);

void CardObj_EmitReadTransferCommand(CardObj *arg0) {
    int state = arg0->field_46;

    switch (state) {
    case 2:
        CardObj_EmitCommand4C(arg0, arg0->field_47);
        break;
    case 3:
        CardObj_EmitCommand46(arg0, arg0->field_47);
        break;
    case 4:
        if (arg0->field_48 == 0) {
            CardObj_EmitCommand47(arg0, arg0->field_47);
        } else {
            CardObj_EmitCommand4B(arg0);
        }
        break;
    }
}
