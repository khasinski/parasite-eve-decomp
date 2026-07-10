#include "include_asm.h"

typedef unsigned char u8;
typedef unsigned short u16;
typedef signed short s16;

extern u16 g_RenderDrawBaseY1;
extern u16 g_RenderDrawBaseY0;
extern u8 g_DrawBaseY;

void Spu_InitHardware(void);
void Akao_Cmd_F0(void);
void Akao_Cmd_F1(void);
void Akao_Cmd_98_9A_9C(int arg0);
extern void VSync(int arg0);
extern int CdRom_ResetDsReadSystem(void);
extern int Cd_GetReadyStatus(void);
int CdRom_GetDiskType(void);
extern short g_CdDiskType;

void Boot_InitMemoryLayout(void);
void Akao_ClearVoiceBank(void);
void Asset_Find08w(int arg0, int arg1, int arg2, int arg3, int arg4);

typedef struct AkaoVoiceActor {
    u8 pad_00[0x0C];
    u8 field_0c;
    u8 field_0d;
    u8 field_0e;
    u8 field_0f;
    u8 pad_10[0x06];
    u16 field_16;
    u8 pad_18[0x02];
    u16 field_1a;
    int field_1c;
    u8 pad_20[0x0A];
    s16 field_2a;
    u8 pad_2c[0x02];
    s16 field_2e;
    u8 pad_30[0x02];
    s16 field_32;
} AkaoVoiceActor;

typedef struct AkaoVoiceBankEntry {
    u8 field_0;
    u8 field_1;
    u8 field_2;
    u8 frame;
    u16 ids[2];
} AkaoVoiceBankEntry;

typedef struct AkaoVoiceBankState {
    int flags;
    u8 pad_04[0x0D];
    u8 first_dynamic_entry;
    u8 active_id_slot;
} AkaoVoiceBankState;

extern AkaoVoiceBankEntry D_80094488[];
extern AkaoVoiceBankState D_800B0CD8;
extern struct { char _[16]; } D_8009D254_o __asm__("D_8009D254");
extern struct { char _[16]; } D_8009D1A0_o __asm__("D_8009D1A0");
#define D_8009D254 (*(AkaoVoiceActor **)&D_8009D254_o)
#define D_8009D1A0 (*(int *)&D_8009D1A0_o)

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

int Akao_LoadVoiceBank(AkaoVoiceActor *actor) {
    register AkaoVoiceActor *actor_reg asm("$17") = actor;
    register int frame asm("$19");
    register int step asm("$22");
    register int target asm("$21");
    register int i asm("$18");
    register int offset asm("$16");
    register AkaoVoiceBankEntry *entry asm("$20");
    register AkaoVoiceBankState *state asm("$23");
    int id;

    if (actor_reg == 0) {
        return -1;
    }

    state = &D_800B0CD8;
    frame = actor_reg->field_16;
    step = *(int *)((char *)actor_reg + 0x1C);
    target = actor_reg->field_1a;

    if (step > 0 && frame < target) {
        frame = frame + 1 + actor_reg->field_0f;
    } else if (step < 0 && target < frame) {
        frame = frame - 1 - actor_reg->field_0f;
    }

    if (actor_reg == D_8009D254 && (D_8009D1A0 & 2) == 0 && (state->flags & 0x800000) == 0) {
        entry = D_80094488;
        for (i = 0; i < 4; i++, entry++) {
            if (entry->field_2 == actor_reg->field_0e) {
                if ((step > 0 && entry->frame >= target && entry->frame < frame) ||
                    (step < 0 && frame < entry->frame && entry->frame <= target)) {
                    id = entry->ids[state->active_id_slot];
                    if (id != 0) {
                        Asset_Find08w(id, 0, *(s16 *)((char *)actor_reg + 0x2A),
                                      *(s16 *)((char *)actor_reg + 0x2E), *(s16 *)((char *)actor_reg + 0x32));
                    }
                }
            }
        }
    }

    i = 4;
    if (i >= state->first_dynamic_entry + 4) {
        return 0;
    }

    entry = &D_80094488[i];
    do {
        if (entry->field_0 == actor_reg->field_0c && entry->field_1 == actor_reg->field_0d &&
            entry->field_2 == actor_reg->field_0e) {
            if ((step > 0 && entry->frame >= target && entry->frame < frame) ||
                (step < 0 && frame < entry->frame && entry->frame <= target)) {
                id = entry->ids[state->active_id_slot];
                if (id != 0) {
                    Asset_Find08w(id, 0, *(s16 *)((char *)actor_reg + 0x2A),
                                  *(s16 *)((char *)actor_reg + 0x2E), *(s16 *)((char *)actor_reg + 0x32));
                }
            }
        }

        i++;
        entry++;
    } while (i < state->first_dynamic_entry + 4);

    return 0;
}

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
