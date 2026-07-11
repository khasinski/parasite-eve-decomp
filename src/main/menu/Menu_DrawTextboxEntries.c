/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
#include "pe1/textbox.h"

typedef unsigned char u8;
typedef unsigned int u32;

extern void *D_8009CE90;
extern u32 D_8009D1F4;
extern int D_8009CED4;
extern unsigned char D_8009CEA8;
extern unsigned char D_8009CEAC;
extern unsigned char D_8009CEB0;
extern unsigned char D_8009CEC4;
extern unsigned char D_8009CEC8;
extern unsigned char D_8009CECC;

void AddPrim(void *ot, void *prim);
void Draw_SetCursor(int x, int y);
void Draw_AllocColorGradient(int width, int height, int arg2, int arg3);
void Draw_PrintRawText(u8 *text);

static void Textbox_SetDefaultColor(void) {
    D_8009CEA8 = 0x80;
    D_8009CEAC = 0x80;
    D_8009CEB0 = 0x80;
}

static void Textbox_SetHighlightColor(void) {
    D_8009CEA8 = D_8009CEC4;
    D_8009CEAC = D_8009CEC8;
    D_8009CEB0 = D_8009CECC;
}

static u8 *Textbox_FindPage(u8 *message, int page_id) {
    int current_page;

    current_page = 0;
    while (message != 0 && *message != 0xFF) {
        if (*message == 0xFE) {
            message++;
            current_page = *message;
            if (current_page == page_id) {
                return message + 1;
            }
        } else if (*message == 0xF9) {
            break;
        }
        message++;
    }
    return message;
}

static void Textbox_DrawLine(u8 *line, int count, int x, int y) {
    u8 buffer[0x80];
    int i;

    if (count <= 0) {
        return;
    }
    if (count > 0x7E) {
        count = 0x7E;
    }

    for (i = 0; i < count; i++) {
        buffer[i] = line[i];
    }
    buffer[count] = 0xFF;

    Draw_SetCursor(x, y);
    Draw_PrintRawText(buffer);
}

static void Textbox_DrawStream(TextboxEntry *entry) {
    u8 *cursor;
    u8 *line_start;
    int line_len;
    int base_x;
    int x;
    int y;
    int safety;

    cursor = entry->message;
    if (cursor == 0) {
        return;
    }

    base_x = entry->x;
    x = base_x;
    y = entry->y;
    line_start = cursor;
    line_len = 0;
    safety = 0x400;

    while (safety-- > 0) {
        u8 code = *cursor++;

        if (code == 0xFF) {
            Textbox_DrawLine(line_start, line_len, x, y);
            if (entry->state == 2 && (D_8009D1F4 & 0x100) != 0 && (entry->flags & 0x02000000) == 0) {
                entry->state = 0;
            }
            break;
        }

        if (code == 0xF9) {
            Textbox_DrawLine(line_start, line_len, x, y);
            entry->state = 0;
            entry->message = cursor;
            break;
        }

        if (code == 0xF7) {
            Textbox_DrawLine(line_start, line_len, x, y);
            y += 0x0C;
            x = base_x;
            line_start = cursor;
            line_len = 0;
            continue;
        }

        if (code == 0xF8) {
            Textbox_DrawLine(line_start, line_len, x, y);
            while (*cursor != 0xF8 && *cursor != 0xFF) {
                cursor++;
            }
            if (*cursor == 0xF8) {
                cursor++;
            }
            line_start = cursor;
            line_len = 0;
            continue;
        }

        if (code == 0xFE) {
            if (*cursor == (entry->page_id & 0xFF)) {
                cursor++;
                line_start = cursor;
                line_len = 0;
            } else {
                cursor++;
            }
            continue;
        }

        if (code == 0xFA) {
            line_len++;
            continue;
        }

        if (code >= 0xF0) {
            continue;
        }

        line_len++;
    }
}

void Menu_DrawTextboxEntries(void) {
    int i;

    if (D_8009CED4 != 0) {
        Draw_AllocColorGradient(0x140, 0x36, 0, 0);
    }

    for (i = 0; i < 4; i++) {
        TextboxEntry *entry = &g_TextboxEntries[i];

        if (entry->state == 0) {
            continue;
        }

        if (entry->state == 1) {
            entry->message = D_8009CE90;
            entry->flags &= 0xFFF0FFFF;
            if (entry->page_id >= 0) {
                entry->message = Textbox_FindPage(entry->message, entry->page_id);
            }
            entry->state = 2;
        }

        Textbox_SetDefaultColor();
        if (entry->style == 3) {
            Textbox_SetHighlightColor();
        }

        if (entry->style == 1 || entry->style == 3) {
            Draw_AllocColorGradient(entry->color0, entry->color1, 0, 0);
        }

        Textbox_DrawStream(entry);
    }
}
