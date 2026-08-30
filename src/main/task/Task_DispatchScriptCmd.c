#include "common.h"

/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
/* ASPSX_VERSION: 2.70 */


#define U8_AT(ptr, off) (*(u8 *)((u8 *)(ptr) + (off)))
#define U32_AT(ptr, off) (*(u32 *)((u8 *)(ptr) + (off)))

typedef struct ScriptArgParts {
    s16 low;
    signed int high : 16;
} ScriptArgParts;

typedef struct TaskScriptArgs {
    int *opcode;
    ScriptArgParts *arg1;
    ScriptArgParts *arg2;
    ScriptArgParts *arg3;
    ScriptArgParts *arg4;
    ScriptArgParts *arg5;
} TaskScriptArgs;

#define ARG_WORD(ptr) (*(int *)(ptr))
#define ARG_BYTE(ptr) (*(u8 *)(ptr))

typedef struct ScriptPosition {
    s16 x;
    s16 y;
    s16 z;
} ScriptPosition;

typedef struct ScriptMenuByte {
    u8 value;
    u8 pad1[8];
} ScriptMenuByte;

typedef struct ScriptMenuShort {
    s16 value;
    u8 pad2[7];
} ScriptMenuShort;

extern u8 *D_8009D20C[];
extern u8 *D_8009D254[];
extern u8 *D_8009D2F0[];
extern int D_8009CE00;
extern u8 *D_8009D300;
u8 g_ScriptCameraBytes[3] __asm__("D_8009CDF8");
extern u32 D_800B0CD8[];
extern ScriptMenuByte g_ScriptMenuValue0 __asm__("D_800BD025");
extern ScriptMenuByte g_ScriptMenuValue1 __asm__("D_800BD026");
extern ScriptMenuByte g_ScriptMenuValue2 __asm__("D_800BD027");
extern ScriptMenuShort g_ScriptMenuPositionX __asm__("D_800BD028");
extern ScriptMenuShort g_ScriptMenuPositionY __asm__("D_800BD02A");

void Render_UpdateScrollPosition(void *position, int x, int y);
void Task_SetCollisionFlag(int value);
void func_800E00CC(ScriptPosition *position, int mode, int arg2, int arg3,
                   int arg4, int arg5, int arg6);
u8 *Scene_LoadMap(char *map_id, u8 *entity, int mode);
void Battle_DrawActiveStatus(void);
void Window_SetBoundsByMode(int mode);
int Asset_LoadTimTextures(int mode);

int Task_DispatchScriptCmd(TaskScriptArgs *args)
{
    ScriptPosition position;
    ScriptPosition position2;
    char map_id[2];
    u8 *entity;
    u8 *loaded;
    int opcode;
    int status;
    int entity_id;
    int initial_entity_id;

    opcode = *args->opcode;
    if (opcode == 0x9C4) {
        goto cmd_9c4;
    }
    if (opcode < 0x9C5) {
        if (opcode == 0x899) {
            goto cmd_899;
        }
        if (opcode < 0x89A) {
            if (opcode == 0x835) {
                goto cmd_835;
            }
            if (opcode < 0x836) {
                goto done;
            }
            if (opcode == 0x898) {
                goto cmd_898;
            }
            goto done;
        }
        if (opcode == 0x960) {
            goto cmd_960;
        }
        if (opcode < 0x961) {
            if (opcode == 0x8FC) {
                goto cmd_8fc;
            }
            goto done;
        }
        if (opcode == 0x961) {
            goto cmd_961;
        }
        if (opcode == 0x962) {
            goto cmd_962;
        }
        goto done;
    }
    if (opcode == 0xAF1) {
        goto cmd_af1;
    }
    if (opcode < 0xAF2) {
        if (opcode == 0xA29) {
            goto cmd_a29;
        }
        if (opcode < 0xA2A) {
            if (opcode == 0xA28) {
                goto cmd_a28;
            }
            goto done;
        }
        if (opcode == 0xA8C) {
            goto cmd_a8c;
        }
        if (opcode == 0xAF0) {
            goto cmd_af0;
        }
        goto done;
    }
    if (opcode == 0xBB8) {
        goto find_entity;
    }
    if (opcode < 0xBB9) {
        if (opcode == 0xB54) {
            goto cmd_b54;
        }
        goto done;
    }
    if (opcode == 0xC1C) {
        goto cmd_c1c;
    }
    if (opcode == 0xC80) {
        goto cmd_c80;
    }
    goto done;

cmd_835:
        D_800B0CD8[0] |= 0x800;
        goto done;
cmd_898:
        Render_UpdateScrollPosition(D_8009D254[0] + 0x28,
                                    ARG_WORD(args->arg1), ARG_WORD(args->arg2));
        goto done;
cmd_899:
        g_ScriptMenuPositionX.value = ARG_WORD(args->arg1);
        g_ScriptMenuPositionY.value = ARG_WORD(args->arg2);
        goto done;
cmd_8fc:
        Task_SetCollisionFlag(ARG_WORD(args->arg1) != 0);
        goto done;
cmd_960:
        position.x = args->arg1->high;
        position.y = args->arg2->high;
        position.z = args->arg3->high;
        func_800E00CC(&position, 0, args->arg4->low,
                      ARG_BYTE(args->arg5), 0, 0, 0);
        goto done;
cmd_961:
        g_ScriptCameraBytes[0] = ARG_WORD(args->arg1);
        g_ScriptCameraBytes[1] = ARG_WORD(args->arg2);
        g_ScriptCameraBytes[2] = ARG_WORD(args->arg3);
        goto done;
cmd_962:
        position2.x = args->arg1->high;
        position2.y = args->arg2->high;
        position2.z = args->arg3->high;
        func_800E00CC(&position2, 1, args->arg4->low,
                      ARG_BYTE(args->arg5), g_ScriptCameraBytes[0],
                      g_ScriptCameraBytes[1], g_ScriptCameraBytes[2]);
        goto done;
cmd_9c4:
        map_id[0] = ARG_WORD(args->arg1);
        map_id[1] = ARG_WORD(args->arg2);
        loaded = Scene_LoadMap(map_id, D_8009D2F0[0], 0);
        U32_AT(loaded, 0x28) = ARG_WORD(args->arg3);
        U32_AT(loaded, 0x2C) = ARG_WORD(args->arg4);
        U32_AT(loaded, 0x30) = ARG_WORD(args->arg5);
        goto done;
cmd_a28:
        U8_AT(D_8009D2F0[0], 0x27C) = ARG_WORD(args->arg1);
        goto done;
cmd_a29:
        U8_AT(D_8009D2F0[0], 0x27D) = ARG_WORD(args->arg1);
        goto done;
cmd_a8c:
        g_ScriptMenuValue0.value = ARG_WORD(args->arg1);
        g_ScriptMenuValue1.value = ARG_WORD(args->arg2);
        g_ScriptMenuValue2.value = ARG_WORD(args->arg3);
        goto done;
cmd_af0:
        Battle_DrawActiveStatus();
        goto done;
cmd_af1:
        Window_SetBoundsByMode(ARG_BYTE(args->arg1));
        goto done;
cmd_b54:
        D_800B0CD8[0] |= 0x400000;
        goto done;
find_entity:
        initial_entity_id = ARG_WORD(args->arg1);
        if (initial_entity_id == 0) {
            goto done;
        }
        entity = D_8009D20C[0];
        entity_id = initial_entity_id;
        if (entity == 0) {
            goto done;
        }
find_entity_loop:
        if (U8_AT(entity, 0xC) == entity_id) {
            if (U8_AT(entity, 0xD) == ARG_WORD(args->arg2)) {
                if ((U32_AT(entity, 0x98) & 0x10) == 0) {
                    goto done;
                }
            }
        }
        entity = (u8 *)U32_AT(entity, 4);
        if (entity != 0) {
            goto find_entity_loop;
        }
        goto done;
cmd_c1c:
        status = Asset_LoadTimTextures(1);
        if (status == 1) {
            D_8009CE00 -= 0x28;
            U32_AT(D_8009D300, 0x10) = status;
            return 0;
        }
        goto done;
cmd_c80:
        D_800B0CD8[0] |= 0x8000000;
done:
    return 1;
}
