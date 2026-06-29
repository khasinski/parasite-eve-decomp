extern void Spu_InitHardware(void);
extern void Akao_Cmd_F0(void);
extern void Akao_Cmd_F1(void);
extern void Akao_Cmd_98_9A_9C(int arg0);
extern void VSync(int arg0);
extern int CdRom_ResetDsReadSystem(void);
extern int Cd_GetReadyStatus(void);
extern int CdRom_GetDiskType(void);
extern short g_CdDiskType;

void Sys_SyncShutdown(void) {
    {
        int done = 1;

        Spu_InitHardware();
        Akao_Cmd_F0();
        Akao_Cmd_F1();
        Akao_Cmd_98_9A_9C(0);

        while (CdRom_ResetDsReadSystem() != done) {
            VSync(0);
        }
    }

    {
        int done = 1;

        while (Cd_GetReadyStatus() != done) {
            VSync(0);
        }
    }

    g_CdDiskType = CdRom_GetDiskType();
}
