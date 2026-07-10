/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
#include "include_asm.h"

extern int D_8009D124;
extern int D_8009D128;
extern int *D_8009D12C;
extern int D_800A2270[];
extern int D_800A22B0[];
extern int D_8009D0D8;
extern int D_8009CDB0;

int Draw_LookupGlyphMetrics(int ch);
void Draw_AllocTexturedQuad(int ch);
void BoundsCheck_AssertStub(int arg0);

void Draw_PrintCenteredTextInWidth(unsigned char *text, int width) {
    register unsigned char *scan asm("$18");
    register int target_width asm("$19");
    register unsigned char *cursor asm("$16");
    register int text_width asm("$17");
    register int *push_stack asm("$5");
    register int *draw_stack asm("$4");
    register int *pop_stack asm("$3");
    int ch;
    int mapped;
    int glyph_width;
    int saved_x;
    int saved_y;

    scan = text;
    target_width = width;

    push_stack = D_8009D12C;
    if (push_stack < D_800A22B0) {
        saved_x = D_8009D124;
        saved_y = D_8009D128;
        D_8009D12C = push_stack + 2;
        push_stack[0] = saved_x;
        push_stack[1] = saved_y;
    } else {
        BoundsCheck_AssertStub(2);
    }

    cursor = scan;
    text_width = 0;
    if (*scan != 0xFF) {
        do {
            ch = *cursor;
            cursor++;
            mapped = ch & 0xFF;
            if (D_8009D0D8 != 0) {
                mapped += D_8009D0D8 << 8;
                D_8009D0D8 = 0;
            }

            if ((unsigned int)(ch & 0xFF) >= 0xFA) {
                D_8009D0D8 = (ch & 0xFF) - 0xFA;
                mapped = -1;
            }

            if (mapped >= 0) {
                D_8009CDB0 = 1;
                if (mapped >= 0xA && mapped != 0xF) {
                    D_8009CDB0 = 0;
                }
                D_8009CDB0++;

                if (mapped >= 0x100) {
                    mapped -= 0x13;
                }

                glyph_width = Draw_LookupGlyphMetrics(mapped);
                text_width += ((glyph_width >> 4) & 0xF) + D_8009CDB0;
            }
        } while (*cursor != 0xFF);
        cursor = scan;
    }

    D_8009D124 += (target_width - (text_width + 4)) >> 1;
    D_8009D128 = D_8009D128;

    if (cursor != 0) {
        draw_stack = D_8009D12C;
        if (draw_stack < D_800A22B0) {
            draw_stack[0] = D_8009D124;
            draw_stack[1] = D_8009D128;
            D_8009D12C = draw_stack + 2;
        } else {
            BoundsCheck_AssertStub(2);
        }

        ch = *cursor;
        if ((ch & 0xFF) != 0xFF) {
            do {
                Draw_AllocTexturedQuad(ch);
                cursor++;
                ch = *cursor;
            } while (ch != 0xFF);
        }

        pop_stack = D_8009D12C;
        if (D_800A2270 < pop_stack) {
            D_8009D12C = pop_stack - 2;
            D_8009D124 = pop_stack[-2];
            D_8009D128 = pop_stack[-1];
        } else {
            BoundsCheck_AssertStub(3);
        }
    }

    pop_stack = D_8009D12C;
    if (D_800A2270 < pop_stack) {
        D_8009D12C = pop_stack - 2;
        D_8009D124 = pop_stack[-2];
        D_8009D128 = pop_stack[-1];
    } else {
        BoundsCheck_AssertStub(3);
    }
}
