typedef short s16;

void func_80085644(void);
void Akao_Cmd_F0(void);
void Akao_Cmd_F1(void);
void Akao_Cmd_98_9A_9C(int arg0);
int CdRom_ResetDsReadSystem(void);
int Cd_GetReadyStatus(void);
int CdRom_GetDiskType(void);
void VSync(int mode);

extern s16 D_800B0DD4;

void func_8006A5BC(void)
{
    func_80085644();
    Akao_Cmd_F0();
    Akao_Cmd_F1();
    Akao_Cmd_98_9A_9C(0);

    while (CdRom_ResetDsReadSystem() != 1) {
        VSync(0);
    }

    while (Cd_GetReadyStatus() != 1) {
        VSync(0);
    }

    D_800B0DD4 = CdRom_GetDiskType();
}
