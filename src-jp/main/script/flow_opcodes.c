/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
/* MASPSX_FORCE_G0: 1 */

#include "common.h"
#include "pe1/battle_entity_anim.h"
#include "pe1/battle_runtime.h"
#include "pe1/battle_update.h"
#include "pe1/entity_allocation.h"
#include "pe1/entity_animation.h"
#include "pe1/field_actor.h"
#include "pe1/field_collision.h"
#include "pe1/field_sfx.h"
#include "pe1/game_state.h"
#include "pe1/game_state_types.h"
#include "pe1/geom_state.h"
#include "pe1/menu_equipment.h"
#include "pe1/menu_state.h"
#include "pe1/player_entity.h"
#include "pe1/render_lighting.h"
#include "pe1/render_shadow.h"
#include "pe1/scene_room_load.h"
#include "pe1/scene_transition.h"
#include "pe1/script_node.h"
#include "pe1/script_queue.h"
#include "pe1/script_state.h"
#include "pe1/textbox.h"
#include "pe1/vector_types.h"

/* COMMON supplies small-data metadata to MASPSX: these words are reached
 * through $gp, every other extern stays absolute. */
u32 *g_ScriptCursor;
ScriptNode *g_ScriptNodePool;
u8 g_ScriptCameraBytes[3];
int g_ScriptCursorAddress;

typedef struct ScriptArgParts {
    s16 low;
    signed int high : 16;
} ScriptArgParts;

typedef struct ScriptCmdArgs {
    int *opcode;
    ScriptArgParts *arg1;
    ScriptArgParts *arg2;
    ScriptArgParts *arg3;
    ScriptArgParts *arg4;
    ScriptArgParts *arg5;
} ScriptCmdArgs;

#define ARG_WORD(ptr) (*(int *)(ptr))
#define ARG_BYTE(ptr) (*(u8 *)(ptr))

void FieldFx_SpawnScriptSprite(Pe1Vec3s *position, int mode, int arg2, int arg3,
                   int arg4, int arg5, int arg6);

int Script_DispatchScriptCmd(ScriptCmdArgs *args)
{
    Pe1Vec3s position;
    Pe1Vec3s position2;
    char map_id[2];
    FieldActor *entity;
    FieldActor *loaded;
    int status;
    int entity_id;
    int initial_entity_id;

    /* Opcode 2100 is accepted and does nothing; its case node is what
     * makes the compiler split the dispatch tree at 2500 as retail does. */
    switch (*args->opcode) {
    case 2100:
        break;
    case 2101:
        g_GameState.flags |= 0x800;
        break;
    case 2200:
        Render_UpdateScrollPosition(&(g_PlayerEntity)->pos_x,
                                    ARG_WORD(args->arg1), ARG_WORD(args->arg2));
        return 1;
    case 2201:
        g_GeomScroll.state.position.originX = ARG_WORD(args->arg1);
        g_GeomScroll.state.position.originY = ARG_WORD(args->arg2);
        break;
    case 2300:
        Script_SetCollisionFlag(ARG_WORD(args->arg1) != 0);
        return 1;
    case 2400:
        position.x = args->arg1->high;
        position.y = args->arg2->high;
        position.z = args->arg3->high;
        FieldFx_SpawnScriptSprite(&position, 0, args->arg4->low,
                      ARG_BYTE(args->arg5), 0, 0, 0);
        return 1;
    case 2401:
        g_ScriptCameraBytes[0] = ARG_WORD(args->arg1);
        g_ScriptCameraBytes[1] = ARG_WORD(args->arg2);
        g_ScriptCameraBytes[2] = ARG_WORD(args->arg3);
        break;
    case 2402:
        position2.x = args->arg1->high;
        position2.y = args->arg2->high;
        position2.z = args->arg3->high;
        FieldFx_SpawnScriptSprite(&position2, 1, args->arg4->low,
                      ARG_BYTE(args->arg5), g_ScriptCameraBytes[0],
                      g_ScriptCameraBytes[1], g_ScriptCameraBytes[2]);
        return 1;
    case 2500:
        map_id[0] = ARG_WORD(args->arg1);
        map_id[1] = ARG_WORD(args->arg2);
        loaded = Scene_LoadMap((u8 *)map_id, g_CurrentEntity, 0);
        loaded->pos_x = ARG_WORD(args->arg3);
        loaded->pos_y = ARG_WORD(args->arg4);
        loaded->pos_z = ARG_WORD(args->arg5);
        break;
    case 2600:
        ((RenderShadowActor *)g_CurrentEntity)->shadow_matrix_index = ARG_WORD(args->arg1);
        break;
    case 2601:
        ((RenderShadowActor *)g_CurrentEntity)->shadow_intensity = ARG_WORD(args->arg1);
        break;
    case 2700:
        g_RoomLightTint.r = ARG_WORD(args->arg1);
        g_RoomLightTint.g = ARG_WORD(args->arg2);
        g_RoomLightTint.b = ARG_WORD(args->arg3);
        break;
    case 2800:
        Battle_DrawActiveStatus();
        return 1;
    case 2801:
        Battle_SetWindowBoundsByMode(ARG_BYTE(args->arg1));
        return 1;
    case 2900:
        g_GameState.flags |= 0x400000;
        break;
    case 3000:
        initial_entity_id = ARG_WORD(args->arg1);
        if (initial_entity_id == 0) {
            return 1;
        }
        entity = g_FieldActorListHead;
        entity_id = initial_entity_id;
        if (entity == 0) {
            return 1;
        }
        while (1) {
            if (entity->type_id == entity_id) {
                if (entity->sub_id == ARG_WORD(args->arg2)) {
                    if ((entity->flags & 0x10) == 0) {
                        return 1;
                    }
                }
            }
            entity = entity->next;
            if (entity == 0) {
                break;
            }
        }
        return 1;
    case 3100:
        status = Asset_LoadTimTextures(1);
        if (status == 1) {
            g_ScriptCursor -= 10;
            g_ScriptNodePool->active = status;
            return 0;
        }
        return 1;
    case 3200:
        g_GameState.flags |= 0x8000000;
        break;
    }
    return 1;
}

int Script_SetEntityShadowFlag(void) {
    g_CurrentEntity->flags |= 0x20000000;
    return 1;
}

int Script_ClearEntityShadowFlag(void) {
    g_CurrentEntity->flags &= 0xDFFFFFFF;
    return 1;
}

int Script_BeginSceneTransition(int **arg0) {
    Pe1GameState *state = &g_GameState;
    int **saved = arg0;
    ScriptNode *node;
    unsigned short *node_flags;
    int flags;
    int cursor;

    if (!(state->flags & 0x1000)) {

    node = g_ScriptNodePool;
    node_flags = &node->flags;
    flags = *node_flags;
    if ((flags & 0x20) == 0) {
        *node_flags = flags | 0x20;
        cursor = g_ScriptCursorAddress;
        node->active = 1;
        cursor -= 0xC;
        g_ScriptCursorAddress = cursor;
        return 0;
    }

    Render_BeginSceneLoad();
    Menu_OpenEquipScreen(**saved);
    {
        ScriptNode *tail_node = g_ScriptNodePool;
        int tail_flags;

        state->flags |= 0x9000;
        tail_flags = tail_node->flags;
        g_GameStateFlags |= 4;
        tail_flags &= 0xFFDF;
        tail_node->flags = tail_flags;
    }

    }
ret_one:
    return 1;
}

void Menu_OpenSaveLoadEntryPoint(int arg0);

int Script_OpenSaveLoadMenu(int **arg0) {
    Pe1GameState *state = &g_GameState;
    ScriptNode *node;
    int flags;

    if (!(state->flags & 0x1000)) {
        node = g_ScriptNodePool;
        flags = node->flags;
        if (!(flags & 0x20)) {
            node->flags = flags | 0x20;
            node->active = 1;
            g_ScriptCursorAddress -= 0xC;
            return 0;
        }
        Render_BeginSceneLoad();
        Menu_OpenSaveLoadEntryPoint(**arg0);
        state->flags |= 0x9000;
        g_GameStateFlags |= 4;
        g_ScriptNodePool->flags &= 0xFFDF;
    }
    return 1;
}

int Script_GetPlayerInputFlag(int **args) {
    if (D_8009CDA8 & 1) {
        *args[0] = 0;
    } else {
        *args[0] = 1;
    }

    return 1;
}

/* Script interpreter main loop and the jump-to-actor-script opcode; a
 * contiguous -G8 pair sharing the script cursor g_ScriptCursor. */

PE1_STATIC_ASSERT(sizeof(ScriptNode) == 0x2C, interpreter_script_node_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(ScriptNode, current) == 0, interpreter_script_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(ScriptNode, active) == 0x10, interpreter_wait_offset);

/* Tentative COMMON declarations preserve small-data metadata for stock
 * maspsx. Storage remains at the existing data/linker addresses. */
ScriptNode *g_ScriptNodePool;
u32 *g_ScriptCursor;

void Script_RunQueue(void)
{
    u32 *args[16];
    ScriptNode *node;
    u8 *entity;
    u32 header, modes, tail, opcode;
    u32 *values;
    u8 *argument_entity;
    u16 i;

    do {
        node = g_ScriptNodePool;
        /* Retail first reads flags and sequence together as a word. */
        if (*(u32 *)&node->flags & 0x50) continue;
        entity = (u8 *)g_CurrentEntity;
        if ((((FieldActor *)entity)->flags & 0x1000) && !(node->flags & 0x80)) continue;
        if ((g_GameStateFlags & 0x100) && entity != (u8 *)D_8009CD10 && !(node->flags & 0x80)) continue;
        node = g_ScriptNodePool;
        if (node->active == 0) continue;
        node->active--;
        if (node->active != 0) continue;
        g_ScriptCursor = node->current.script;
        do {
            header = *g_ScriptCursor;
            g_ScriptCursor++;
            values = g_ScriptCursor + 1;
            tail = *g_ScriptCursor;
            modes = header >> 17;
            opcode = header & 0x1FFF;
            header >>= 13;
            header &= 15;
            g_ScriptCursor = values + header;
            for (i = 0; i < header; i++) {
                switch (modes & 7) {
                case 0: args[i] = &values[i]; break;
                case 3: args[i] = &g_ScriptOperandTable[values[i]]; break;
                case 1:
                    /* Reload after every handler: it may switch actors. */
                    args[i] = (u32 *)((argument_entity = D_8009CDB0[0]) + 0xAC + values[i] * 4);
                    break;
                case 2: args[i] = &g_ScriptVariableBank[values[i]]; break;
                case 4: args[i] = &g_ScriptNodeActiveFlags[values[i]]; break;
                }
                modes >>= 3;
                if (i == 4) modes = tail;
            }
        } while (g_ScriptQueueOpcodeHandlers[opcode](args));
        /* Handlers may replace both the current node and the script cursor. */
        g_ScriptNodePool->current.script = g_ScriptCursor;
    } while ((g_ScriptNodePool = g_ScriptNodePool->next) != 0);
}

typedef struct {
    int value;
} ScriptJumpOffsetEntry;

int Script_JumpToEntityOffset(ScriptJumpOffsetEntry **arg0)
{
    g_ScriptCursor = (u32 *)(g_CurrentEntity->script_base + ((*arg0)->value * 2));
    return 1;
}

int Entity_MarkCurrentDead(void) {
    g_CurrentEntity->flags |= 0x10;
    return 0;
}

int Entity_SetCurrentNodeActive(int **arg0) {
    g_ScriptNodePool->active = *(unsigned short *)arg0[0];
    return 0;
}

int Entity_MarkCurrentNodeFree(void) {
    g_ScriptNodePool->flags |= 0x10;
    return 0;
}

int Script_JumpIfZero(int **arg0) {
    if (*arg0[0] == 0) {
        g_ScriptCursorAddress = (int)(g_CurrentEntity->script_base + (*arg0[1] * 2));
    }
    return 1;
}

int Script_SpawnEntityAt(int **arg0) {
    char local[2];
    FieldActor *entity;

    local[0] = arg0[0][0];
    local[1] = arg0[1][0];
    entity = Scene_LoadMap((u8 *)local, g_CurrentEntity, 1);
    entity->pos_x = arg0[2][0];
    entity->pos_y = arg0[3][0];
    entity->pos_z = arg0[4][0];
    Entity_FindFloor((struct BattleEntity *)entity);
    return 1;
}
int Script_AssignValue(int **arg0) {
    *arg0[0] = *arg0[1];
    return 1;
}

int Script_StopEntityAnim(s16 **arg0) {
    s16 value[5];

    value[0] = -1;
    Render_SetupColorTable(*arg0[0], 0, value);
    return 1;
}

int Math_FixedMul(int arg0, int arg1);

int Entity_ComputeVelocityToTarget(int **arg0) {
    FieldActor *state;
    int delta28;
    int delta2C;
    int delta30;
    int scale;
    int temp;

    state = g_CurrentEntity;
    state->flags |= 2;

    delta28 = state->pos_x - *arg0[0];
    delta2C = state->pos_y - *arg0[1];
    delta30 = state->pos_z - *arg0[2];
    scale = *arg0[3] << 16;

    g_CurrentEntity->motion_x = Math_FixedDivide(delta28, scale);
    g_CurrentEntity->motion_z = Math_FixedDivide(delta30, scale);
    temp = Math_FixedMul(scale + 0x10000, g_CurrentEntity->gravity_y) >> 1;
    g_CurrentEntity->motion_y = temp;
    g_CurrentEntity->motion_y = Math_FixedDivide(delta2C, scale) + g_CurrentEntity->motion_y;

    return 1;
}

int Entity_YieldWhileMoving(void) {
    int ret;

    ret = 0;
    if ((g_CurrentEntity->flags & 2) == 0) {
        return 1;
    }

    g_ScriptCursorAddress -= 8;
    g_ScriptNodePool->active = 1;
    return ret;
}
int Entity_SelectFieldSetter(int **arg0) {
    int value;
    int selector;

    value = *arg0[1];
    if (value < 0) {
        selector = *arg0[0];
        switch (selector) {
        case 1:
            g_CurrentEntity->script_cursor_1a0 = 0;
            break;
        case 2:
            g_CurrentEntity->script_cursor_19c = 0;
            break;
        case 3:
            g_ScriptNodePool->next_value = 0;
            break;
        }
    } else {
        selector = *arg0[0];
        switch (selector) {
        case 1:
            g_CurrentEntity->script_cursor_1a0 = (unsigned int *)(g_CurrentEntity->script_base + (value << 1));
            break;
        case 2:
            g_CurrentEntity->script_cursor_19c = (unsigned int *)(g_CurrentEntity->script_base + (value << 1));
            break;
        case 3:
            g_ScriptNodePool->next_value = (int)(g_CurrentEntity->script_base + (value << 1));
            break;
        }
    }

    return 1;
}

int Entity_SetCurrentFlags(int **args)
{
    g_CurrentEntity->flags |= (*args)[0];
    return 1;
}

int Entity_GetCurrentNodeId(int **args)
{
    *args[0] = g_ScriptNodePool->seq;
    return 1;
}

/* Script opcodes: random value in a range and a field sound effect on the
 * current actor. Contiguous default-profile handlers. */

int Draw_PackPrimColor(int start, int end);

/* Equal bounds return one raw RNG word. Otherwise the result is in [start, end). */
int Script_Random(int **arg0) {
    int start = *arg0[1];
    int end = *arg0[2];

    if (start == end) {
        *arg0[0] = Draw_FlushPrimQueue();
    } else {
        *arg0[0] = Draw_PackPrimColor(start, end);
    }

    return 1;
}

int Script_PlayFieldSfx(FieldSfxScriptArgs *arg0) {
    register int a;
    int b;
    int c;
    int d;
    void *ptr0;
    void *ptr1;
    void *ptr2;

    ptr2 = (void *)arg0->taskArgument;
    ptr1 = (void *)arg0->subId;
    a = *(unsigned char *)ptr2;
    b = *(unsigned char *)ptr1;
    ptr0 = (void *)arg0->typeId;
    c = *(unsigned short *)ptr0;
    d = g_CurrentEntity->field_sfx_id;
    Script_QueueFieldSfx(a, b, c, d, 0);
    return 1;
}

int Draw_GetTextboxPageState(short arg0);

int Script_GetNodeTarget14(int **arg0) {
    *arg0[0] = g_ScriptNodePool->target14;
    return 1;
}

int Script_YieldWhileTableEntrySet(short **arg0) {
    if ((Draw_GetTextboxPageState(*arg0[0]) << 24) != 0) {
        g_ScriptCursorAddress -= 0xC;
        g_ScriptNodePool->active = 1;
        return 0;
    }
    return 1;
}

int Script_ClearTableEntry(short **arg0) {
    Draw_CloseTextboxPage(*arg0[0]);
    return 1;
}

int Script_GetNodeTargets18_1C(int **arg0) {
    *arg0[0] = g_ScriptNodePool->target18.coordinate;
    *arg0[1] = g_ScriptNodePool->target1c;
    return 1;
}

int Entity_FindNodeById(unsigned short **arg0) {
    ScriptNode *node;
    ScriptNode *cur;
    unsigned short id;
    unsigned int bucket;
    unsigned int search_id;
    FieldActor *base;

    id = *arg0[0];
    node = g_ScriptNodePool;
    if (node->seq == id) {
        node->flags |= 0x10;
        return 0;
    }

    base = g_CurrentEntity;
    search_id = id;
    bucket = 0;
    do {
        cur = base->script_node_lists[bucket & 0xFF];
        while (cur != 0) {
            if (cur->seq == search_id) {
                cur->flags |= 0x10;
                return 1;
            }
            cur = cur->next;
        }
        bucket++;
    } while ((bucket & 0xFF) < 3);

    return 1;
}

int Script_GetCurrentEntityFieldD(int **arg0) {
    *arg0[0] = g_CurrentEntity->sub_id;
    return 1;
}

int Script_SetCurrentEntityFieldD(int **arg0) {
    g_CurrentEntity->sub_id = *arg0[0];
    return 1;
}

int Script_GetCurrentEntityField24(int **arg0) {
    *arg0[0] = g_CurrentEntity->field_sfx_id;
    return 1;
}

int Entity_MarkNodeFree(void) {
    u8 i;
    ScriptNode *skip;
    FieldActor *entity;
    ScriptNode *node;

    i = 0;
    entity = g_CurrentEntity;
    skip = g_ScriptNodePool;
    do {
        node = entity->script_node_lists[i];
        while (node != 0) {
            if (node != skip) {
                node->flags |= 0x10;
            }
            node = node->next;
        }
        i++;
    } while (i < 3);

    return 1;
}

int Script_ClearBit(int **arg0) {
    *arg0[0] &= ~(1 << *arg0[1]);
    return 1;
}

int Script_TestBit(int **arg0) {
    *arg0[2] = *arg0[0] & (1 << *arg0[1]);
    return 1;
}

int Script_SetBit(int **arg0) {
    *arg0[0] |= 1 << *arg0[1];
    return 1;
}

int Entity_ClearGlobalFlags(int **arg0) {
    g_FieldMoveLock &= ~*arg0[0];
    return 1;
}

int Entity_GetGlobalFlags(int **arg0) {
    *arg0[0] = g_FieldMoveLock;
    return 1;
}

int Entity_SetGlobalFlags(int **arg0) {
    g_FieldMoveLock |= *arg0[0];
    return 1;
}

int Entity_SetCurrentActionMode(u16 **arg0) {
    FieldActor *entity;

    Entity_SetActionMode((BattleEntity *)g_CurrentEntity, *arg0[0]);
    entity = g_CurrentEntity;
    entity->flags &= ~0x100;
    return 1;
}

int Script_SetEntityAnimFrame(u16 **arg0) {
    FieldActor *entity = g_CurrentEntity;
    u16 value = *arg0[0];

    if (entity->action < value) {
        value = entity->action;
    }

    entity->anim_frame_target = value;
    entity->flags |= 0x200;
    return 1;
}

int Script_WaitAnimFrameMatch(void) {
    ScriptNode *state;
    FieldActor *entity;
    int stack;
    int ret;

    state = g_ScriptNodePool;
    entity = g_CurrentEntity;
    state->active = 1;
    ret = 0;
    if (entity->anim.parts.integer == entity->anim_frame_target) {
        return ret;
    }

    stack = g_ScriptCursorAddress;
    stack -= 8;
    g_ScriptCursorAddress = stack;
    return ret;
}
