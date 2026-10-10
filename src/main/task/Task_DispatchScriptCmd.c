#include "pe1/geom_state.h"
#include "common.h"
#include "pe1/task_node.h"
#include "pe1/vector_types.h"
#include "pe1/render_lighting.h"

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

extern u8 *D_8009D20C[];
extern u8 *D_8009D254[];
extern u8 *D_8009D2F0[];
extern int D_8009CE00;
u8 g_ScriptCameraBytes[3] __asm__("D_8009CDF8");
extern u32 D_800B0CD8[];
/* This absolute view keeps the light outside the interpreter GP window. */
extern RenderLightColor g_ScriptLightColor[] __asm__("D_800BD025");

void Task_SetCollisionFlag(int value);
void func_800E00CC(Pe1Vec3s *position, int mode, int arg2, int arg3,
                   int arg4, int arg5, int arg6);
u8 *Scene_LoadMap(char *map_id, u8 *entity, int mode);
void Battle_DrawActiveStatus(void);
void Window_SetBoundsByMode(int mode);
int Asset_LoadTimTextures(int mode);

int Task_DispatchScriptCmd(TaskScriptArgs *args)
{
    Pe1Vec3s position;
    Pe1Vec3s position2;
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
                return 1;
            }
            if (opcode == 0x898) {
                goto cmd_898;
            }
            return 1;
        }
        if (opcode == 0x960) {
            goto cmd_960;
        }
        if (opcode < 0x961) {
            if (opcode == 0x8FC) {
                goto cmd_8fc;
            }
            return 1;
        }
        if (opcode == 0x961) {
            goto cmd_961;
        }
        if (opcode == 0x962) {
            goto cmd_962;
        }
        return 1;
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
            return 1;
        }
        if (opcode == 0xA8C) {
            goto cmd_a8c;
        }
        if (opcode == 0xAF0) {
            goto cmd_af0;
        }
        return 1;
    }
    if (opcode == 0xBB8) {
        goto find_entity;
    }
    if (opcode < 0xBB9) {
        if (opcode == 0xB54) {
            goto cmd_b54;
        }
        return 1;
    }
    if (opcode == 0xC1C) {
        goto cmd_c1c;
    }
    if (opcode == 0xC80) {
        goto cmd_c80;
    }
    return 1;

cmd_835:
        D_800B0CD8[0] |= 0x800;
        goto done;
cmd_898:
        Render_UpdateScrollPosition(D_8009D254[0] + 0x28,
                                    ARG_WORD(args->arg1), ARG_WORD(args->arg2));
        goto done;
cmd_899:
        D_800BCF88.state.position.originX = ARG_WORD(args->arg1);
        D_800BCF88.state.position.originY = ARG_WORD(args->arg2);
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
        g_ScriptLightColor[0].r = ARG_WORD(args->arg1);
        g_ScriptLightColor[0].g = ARG_WORD(args->arg2);
        g_ScriptLightColor[0].b = ARG_WORD(args->arg3);
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
            return 1;
        }
        entity = D_8009D20C[0];
        entity_id = initial_entity_id;
        if (entity == 0) {
            return 1;
        }
        while (1) {
            if (U8_AT(entity, 0xC) == entity_id) {
                if (U8_AT(entity, 0xD) == ARG_WORD(args->arg2)) {
                    if ((U32_AT(entity, 0x98) & 0x10) == 0) {
                        return 1;
                    }
                }
            }
            entity = (u8 *)U32_AT(entity, 4);
            if (entity == 0) {
                break;
            }
        }
        goto done;
cmd_c1c:
        status = Asset_LoadTimTextures(1);
        if (status == 1) {
            D_8009CE00 -= 0x28;
            D_8009D300->active = status;
            return 0;
        }
        goto done;
cmd_c80:
        D_800B0CD8[0] |= 0x8000000;
done:
    return 1;
}
