#include "pe1/render_object.h"

void FieldEng_UpdatePointHistory(RenderHistoryPoint *history, s16 count,
                                 RenderHistoryPoint *value, int reset) {
    RenderHistoryPoint *cursor;
    int i;

    if (reset) {
        count++;
        for (i = 0; i < count; i++) {
            history->vector.x = value->vector.x;
            history->vector.y = value->vector.y;
            history->vector.z = value->vector.z;
            history++;
        }
    } else {
        cursor = history + (count - 1);
        for (i = 0; i < count;) {
            i++;
            cursor[1].words[0] = cursor[0].words[0];
            cursor[1].words[1] = cursor[0].words[1];
            cursor--;
        }
        history->words[0] = value->words[0];
        history->words[1] = value->words[1];
    }
}
