/* PSY-Q LIBPAD PADPORTD, part 2 of 4: CardObj_ResetFields. */
#include "pe1/card_obj.h"

void CardObj_ResetFields(CardObj *arg0) {
    int count;
    int value;
    char *ptr;

    if (arg0->communicationState != 0) {
        ptr = (char *)arg0->actuatorMap;
        value = 0xFF;
        count = 5;
        arg0->communicationState = 0;
        arg0->field_46 = 0;
        arg0->field_e6 = 0;
        arg0->fn_14 = 0;
        arg0->fn_18 = 0;
        arg0->modeCount = 0;
        arg0->field_e4 = 0;
        arg0->field_e6 = 0;
        arg0->actuatorCount = 0;
        arg0->combinationCount = 0;
        arg0->modeTable = 0;
        arg0->capabilities = 0;
        arg0->combinations = 0;

        do {
            *ptr = value;
            count -= 1;
            ptr += 1;
        } while (count >= 0);
    }
}
