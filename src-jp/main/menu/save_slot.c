/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
/* MASPSX_FORCE_G0: 1 */

#include "common.h"
#include "pe1/save.h"
#include "pe1/textbox.h"
#include "save_slot_metadata.h"

/* COMMON supplies small-data metadata to MASPSX: these words are reached
 * through $gp, every other extern stays absolute. */
unsigned char g_MenuActiveMode;
unsigned char g_SaveOverlayTimer;
u8 g_BattleSaveOverlayActive;
u8 *g_CurItemEffectData;

void Save_DrawSlotMetadata(void)
{
    u32 state;
    u32 next;
    int word;
    int phase;
    int prompt;
    short colors[3];

    if (g_BattleScanPanel.summary == 0 || *g_BattleScanPanel.summary == 0 || (*g_BattleScanPanel.summary)->primaryValue <= 0) {
        Draw_ResetTextboxes();
        g_BattleScanPanel.prompt.word &= ~0x300;
    }

    state = g_BattleScanPanel.prompt.word;
    phase = (state >> 8) & 3;
    switch (phase) {
    case 1:
        colors[0] = (*g_BattleScanPanel.summary)->primaryValue;
        colors[1] = (*g_BattleScanPanel.summary)->secondaryValue;
        colors[2] = 0;
        Draw_ResetTextboxes();
        Menu_SetTextCursorRect(Save_GetMetadataWindowIndex() != 0 ? 0x14 : 0x61, g_MenuActiveMode < 2 ? 0xF : 0xC3, 0, 0);
        Render_SetupColorTable(0, 2, colors);
        prompt = (g_BattleScanPanel.prompt.word >> 10) & 3;
        if (prompt != 0) {
            if (prompt == phase) {
                g_TextboxEntries[0].message = g_AnalyzeHpFormatText;
            }
        } else {
            g_TextboxEntries[0].message = g_AnalyzeHpUnknownText;
        }
        g_TextboxEntries[0].state = 2;
        g_TextboxEntries[0].control.flags |= 0x2000000;
        next = g_BattleScanPanel.prompt.word;
        word = next & ~0x300;
        word |= ((((next >> 8) & 3) + 1) & 3) << 8;
        g_BattleScanPanel.prompt.word = word;
        break;
    case 2:
        if (g_BattleScanPanel.prompt.timer == 0) {
            g_BattleScanPanel.prompt.word = (state & ~0x300) | 0x200;
            g_BattleScanPanel.prompt.timer = 0x4B;
            state = g_BattleScanPanel.prompt.word;
            if (state & 0x1000) {
                g_TextboxEntries[0].message = g_TextSusceptibleToPoison[Save_GetMetadataWindowIndex()];
                g_BattleScanPanel.prompt.word &= ~0x1000;
            } else if (state & 0x2000) {
                g_TextboxEntries[0].message = g_TextSusceptibleToAcid[Save_GetMetadataWindowIndex()];
                g_BattleScanPanel.prompt.word &= ~0x2000;
            } else if (state & 0x4000) {
                g_TextboxEntries[0].message = g_AnalyzeTranquilizerWeakTexts[Save_GetMetadataWindowIndex()];
                g_BattleScanPanel.prompt.word &= ~0x4000;
            } else if (state & 0x8000) {
                g_TextboxEntries[0].message = g_AnalyzeHeatWeakTexts[Save_GetMetadataWindowIndex()];
                g_BattleScanPanel.prompt.word &= ~0x8000;
            } else if (state & 0x10000) {
                g_TextboxEntries[0].message = g_AnalyzeColdWeakTexts[Save_GetMetadataWindowIndex()];
                g_BattleScanPanel.prompt.word &= ~0x10000;
            } else if (state & 0x20000) {
                g_TextboxEntries[0].message = g_AnalyzeStealWeakTexts[Save_GetMetadataWindowIndex()];
                g_BattleScanPanel.prompt.word &= ~0x20000;
            } else if (state & 0x40000) {
                g_TextboxEntries[0].message = g_AnalyzeConfusionWeakTexts[Save_GetMetadataWindowIndex()];
                g_BattleScanPanel.prompt.word &= ~0x40000;
            } else {
                Draw_ResetTextboxes();
                g_BattleScanPanel.prompt.word &= ~0x300;
            }
        }
        break;
    }
    g_BattleScanPanel.prompt.timer--;
    Draw_SetCursor(0, g_MenuActiveMode < 2 ? 0xB : 0xBF);
    Draw_AllocColorGradient(0x140, 0x14, 0, 0);
}

void Menu_SaveOverlayDraw(void)
{
    short color;
    int y;
    int value;
    switch (g_BattleSaveOverlayActive) {
    case 1:
        color = -1;
        Draw_ResetTextboxes();
        value = Save_GetMetadataWindowIndex() ? 20 : 97;
        y = g_MenuActiveMode < 2 ? 15 : 195;
        Menu_SetTextCursorRect(value, y, 0, 0);
        Render_SetupColorTable(0, 2, &color);
        value = g_BattleSaveOverlayActive;
        g_TextboxEntries[0].state = 2;
        g_SaveOverlayTimer = 75;
        g_TextboxEntries[0].message = g_CurItemEffectData;
        value++;
        g_BattleSaveOverlayActive = value;
        g_TextboxEntries[0].control.flags |= 0x2000000;
        break;
    case 2:
        if (!g_SaveOverlayTimer) {
            Draw_ResetTextboxes();
            g_BattleSaveOverlayActive = 0;
        }
        break;
    }
    --g_SaveOverlayTimer;
    Draw_SetCursor(0, g_MenuActiveMode < 2 ? 11 : 191);
    Draw_AllocColorGradient(320, 20, 0, 0);
}
