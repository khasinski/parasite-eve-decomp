/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: --use-comm-section -G8 */
#include "pe1/draw_state.h"
#include "pe1/text.h"

extern int D_8009D164;

void MenuWidget_DrawCenteredTableText(int text_id) {
    Draw_PrintCenteredTextInWidth(Str_LookupTable4(text_id), D_8009D164);
}
