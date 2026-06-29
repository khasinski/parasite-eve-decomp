extern int g_GameStateFlags;

void Render_SetGteScreenOffset(void);
void Render_ResetGteScreenOffset(void);
void Pm_Start(int arg0);
void Pm_Exec(int arg0);

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
