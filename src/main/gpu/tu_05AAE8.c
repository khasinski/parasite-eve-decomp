#include "include_asm.h"

typedef unsigned char u8;
typedef unsigned short u16;
extern u16 g_RenderDrawBaseY1;
extern u16 g_RenderDrawBaseY0;
extern u8 g_DrawBaseY;

extern void Spu_InitHardware(void);
extern void Akao_Cmd_F0(void);
extern void Akao_Cmd_F1(void);
extern void Akao_Cmd_98_9A_9C(int arg0);
extern void VSync(int arg0);
extern int CdRom_ResetDsReadSystem(void);
extern int Cd_GetReadyStatus(void);
extern int CdRom_GetDiskType(void);
extern short g_CdDiskType;

void Boot_InitMemoryLayout(void);
void Akao_ClearVoiceBank(void);

int Draw_SetBaseY(int arg0, unsigned int arg1)
{
  int new_var;
  new_var = arg1 < 0x10;
  if (new_var)
  {
    register int value;
    value = arg1;
    g_DrawBaseY = (g_RenderDrawBaseY0 = (g_RenderDrawBaseY1 = value));
  }
  return 0;
}

INCLUDE_ASM("asm/USA/main/nonmatchings/gpu/tu_05AAE8", Akao_LoadVoiceBank);

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

void Boot_InitGameState(void) {
    Boot_InitMemoryLayout();
    Akao_ClearVoiceBank();
}
