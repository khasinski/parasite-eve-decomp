/* Per-frame process-manager drivers: start then execute the eleven primary
 * slots, and the same for the secondary bank. Contiguous default-profile
 * pair sharing the game-state gating flags. */
#include "pe1/pm.h"

extern int g_GameStateFlags;

void Render_SetGteScreenOffset(void);
void Render_ResetGteScreenOffset(void);
int Pm_Start(int slot);
int Pm_Exec(int slot);

int Pm_RunPrimary(void) {
    int i;
    int slot;
    u32 command;

    if (g_GameStateFlags & 0x80) {
        i = 0;
        Render_SetGteScreenOffset();
        do {
            Pm_Start(i);
            i += 1;
        } while (i < 11);
        Render_ResetGteScreenOffset();
        if (!(g_GameStateFlags & 4)) {
            slot = 0;
            do {
                command = g_PmSlotTable[slot].header.command;
                if (!(g_GameStateFlags & 0x100) || command - 0x55 < 0x1E) {
                    Pm_Exec(slot);
                }
                slot += 1;
            } while (slot < 11);
        }
    }
    return 0;
}

int Pm_RunBatch(void) {
    int i;

    if (g_GameStateFlags & 0x80) {
        Render_SetGteScreenOffset();

        for (i = 0; i < 11; i++) {
            Pm_Start(i + 0xB);
        }

        Render_ResetGteScreenOffset();

        if (g_GameStateFlags & 0x104) {
            return 0;
        }

        for (i = 0; i < 11; i++) {
            Pm_Exec(i + 0xB);
        }
    }

    return 0;
}
