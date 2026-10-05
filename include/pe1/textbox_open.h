#ifndef PE1_TEXTBOX_OPEN_H
#define PE1_TEXTBOX_OPEN_H

#include "pe1/textbox.h"

/* Globals read by Render_SetupColorTable when it opens a textbox. */

/* Box rectangle set by Menu_SetTextCursorRect, copied into styled boxes. */
typedef struct TextboxRect {
    s16 x, y, width, height;
} TextboxRect;

extern TextboxRect D_8009CE98;
extern u8 D_8009CEA0;
extern s8 D_8009CEA4;
extern u8 D_8009CED0;

#endif /* PE1_TEXTBOX_OPEN_H */
