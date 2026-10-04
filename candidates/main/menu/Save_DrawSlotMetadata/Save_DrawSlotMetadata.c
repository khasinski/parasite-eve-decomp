/*
 * Save_DrawSlotMetadata (0x8003495C, 1156 bytes): parked typed draft, lev 6.
 * See README.md for the remaining differences.
 */
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G4 */
#include "common.h"
#include "pe1/save.h"
#include "pe1/textbox.h"
#include "pe1/save_slot_metadata.h"

void Save_DrawSlotMetadata(void)
{
    u32 state;
    u32 next;
    int phase;
    int prompt;
    short colors[3];

    if (D_8009D1A8.summary == 0 || *D_8009D1A8.summary == 0 || (*D_8009D1A8.summary)->primaryValue <= 0) {
        Tbl_ResetAll();
        D_8009D1A8.prompt.word &= ~0x300;
    }

    state = D_8009D1A8.prompt.word;
    phase = (state >> 8) & 3;
    switch (phase) {
    case 1:
        colors[0] = (*D_8009D1A8.summary)->primaryValue;
        colors[1] = (*D_8009D1A8.summary)->secondaryValue;
        colors[2] = 0;
        Tbl_ResetAll();
        Menu_SetTextCursorRect(Save_GetMetadataWindowIndex() != 0 ? 0x14 : 0x61,
                               D_8009CE80 < 2 ? 0xF : 0xC3, 0, 0);
        Render_SetupColorTable(0, 2, colors);
        prompt = (D_8009D1A8.prompt.word >> 10) & 3;
        if (prompt != 0) {
            if (prompt == phase) {
                g_TextboxEntries[0].message = D_80091474;
            }
        } else {
            g_TextboxEntries[0].message = D_80091464;
        }
        g_TextboxEntries[0].state = 2;
        next = D_8009D1A8.prompt.word;
        D_8009D1A8.prompt.word = (next & ~0x300) | ((((next >> 8) & 3) + 1) & 3) << 8;
        g_TextboxEntries[0].control.flags |= 0x2000000;
        break;
    case 2:
        if (D_8009D1A8.prompt.timer == 0) {
            D_8009D1A8.prompt.word = (state & ~0x300) | 0x200;
            D_8009D1A8.prompt.timer = 0x4B;
            state = D_8009D1A8.prompt.word;
            if (state & 0x1000) {
                g_TextboxEntries[0].message = D_80091480[Save_GetMetadataWindowIndex()];
                D_8009D1A8.prompt.word &= ~0x1000;
            } else if (state & 0x2000) {
                g_TextboxEntries[0].message = D_800914AC[Save_GetMetadataWindowIndex()];
                D_8009D1A8.prompt.word &= ~0x2000;
            } else if (state & 0x4000) {
                g_TextboxEntries[0].message = D_800914D4[Save_GetMetadataWindowIndex()];
                D_8009D1A8.prompt.word &= ~0x4000;
            } else if (state & 0x8000) {
                g_TextboxEntries[0].message = D_800914FC[Save_GetMetadataWindowIndex()];
                D_8009D1A8.prompt.word &= ~0x8000;
            } else if (state & 0x10000) {
                g_TextboxEntries[0].message = D_80091520[Save_GetMetadataWindowIndex()];
                D_8009D1A8.prompt.word &= ~0x10000;
            } else if (state & 0x20000) {
                g_TextboxEntries[0].message = D_80091544[Save_GetMetadataWindowIndex()];
                D_8009D1A8.prompt.word &= ~0x20000;
            } else if (state & 0x40000) {
                g_TextboxEntries[0].message = D_80091570[Save_GetMetadataWindowIndex()];
                D_8009D1A8.prompt.word &= ~0x40000;
            } else {
                Tbl_ResetAll();
                D_8009D1A8.prompt.word &= ~0x300;
            }
        }
        break;
    }
    D_8009D1A8.prompt.timer--;
    Draw_SetCursor(0, D_8009CE80 < 2 ? 0xB : 0xBF);
    Draw_AllocColorGradient(0x140, 0x14, 0, 0);
}
