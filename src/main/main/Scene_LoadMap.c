#include "common.h"
#include "pe1/field_actor.h"
#include "pe1/scene_assets.h"
#include "pe1/psyq_nop.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: --use-comm-section -G8 */

#define W(p, off) (*(u32 *)((u8 *)(p) + (off)))
#define H(p, off) (*(u16 *)((u8 *)(p) + (off)))
#define B(p, off) (*(u8 *)((u8 *)(p) + (off)))

extern FieldActor *D_8009D2AC;
extern FieldActor *D_8009D20C;
extern FieldActor *D_8009D254;
extern u32 D_8009D224;
extern u16 D_8009D2A6;
extern u32 D_800915DC[];
extern u32 D_800915E0[];
extern u32 D_800B0E70[];
extern u8 D_800B0CE2[];
extern SceneAssetBlob *D_800B0E64[];
extern u8 D_800B161C[];
extern u8 D_800B89F8[];
extern u8 D_800BEA40[];

void Battle_InitEquipLists();
void *Task_AllocNode();
void Entity_SetActionMode();
void *Entity_AllocBlock();
void Render_SetupEntityPrims();
void Render_InitRoomPrimState();
void Render_DrawWithAnim();

FieldActor *Scene_LoadMap(u8 *scene, FieldActor *after, int allocateFull)
{
    FieldActor *actor;
    FieldActor *available;
    FieldActor *afterNext;
    FieldActor *nextFree;
    u8 *allocation;
    u8 *areaType;
    u32 entry;
    u32 i;
    u32 count;
    u32 value;
    volatile u32 *recordWord;
    u8 sceneType;
    u32 serial;
    u32 *row;
    u8 *baseRows;
    u32 *table;
    SceneAssetBlob *archive;
    SceneAssetDirectory *directory;
    SceneBankAssetRecord *bankRows;
    SceneBankAssetRecord *walk;
    struct { u8 *primData; u32 unknown; u32 mode; } renderSetup;

    available = D_8009D2AC;
    if (!available) return 0;
    actor = available;
    nextFree = actor->next;
    /* Preserve the retail load-delay slot. */
    PE1_NOP();
    D_8009D2AC = nextFree;
    if (after) {
        afterNext = after->next;
        actor->prev = after;
        actor->next = afterNext;
        after->next = actor;
        if (actor->next) actor->next->prev = actor;
    } else {
        if (D_8009D20C) D_8009D20C->prev = actor;
        actor->prev = 0;
        actor->next = D_8009D20C;
        D_8009D20C = actor;
    }

    actor->gravity_x = 0;
    actor->gravity_y = 0x4000;
    actor->gravity_z = 0;
    actor->motion_x = 0;
    actor->motion_y = 0;
    actor->motion_z = 0;
    actor->accel_x = 0;
    actor->accel_y = 0;
    actor->accel_z = 0;
    actor->delta_x = 0;
    actor->delta_y = 0;
    actor->delta_z = 0;

    W(actor, 0x190) = D_800915DC[scene[0] * 2];
    W(actor, 0x194) = D_800915E0[scene[0] * 2];
    if (scene[0] == 0) {
        D_8009D254 = actor;
        actor->move_factor = 0x10000;
        Battle_InitEquipLists(actor);
    } else {
        actor->move_factor = 0x50000;
        actor->state = 0;
    }
    actor->type_id = scene[0];
    actor->sub_id = scene[1];
    if (scene[0] < 10) {
        actor->allocation_active = D_800B0E70[scene[0]];
    } else {
        actor->allocation_active = 0;
    }
    serial = D_8009D224;
    actor->flags = 0x10000000;
    actor->anim_step = 0x10000;
    H(actor, 0x10) = 0xC8;
    actor->action_data = 0;
    actor->anim.fixed = 0;
    actor->parent = 0;
    actor->field_1a4 = 0;
    actor->script_cursor_19c = 0;
    actor->script_cursor_1a0 = 0;
    W(actor, 0x198) = 0;
    actor->move_speed = 0x1000;
    D_8009D224 = serial + 1;
    H(actor, 0x24) = serial;
    actor->mode = 0;
    B(actor, 0x27C) = 0;
    B(actor, 0x27D) = 0x80;
    for (i = 0; i < 0x38; ++i) W(actor, 0xAC + 4 * i) = 0;
    for (i = 0; i < 3; ++i) W(actor, 0xA0 + 4 * i) = 0;

    table = *(u32 **)D_800B161C;
    actor->script_base = (u8 *)table[2 + scene[0]];
    actor->task_node_lists[2] = Task_AllocNode(actor->script_base, 0);
    actor->rot_x = 0;
    actor->rot_y = 0;
    actor->rot_z = 0;
    D_8009D2A6++;

    if (!actor->allocation_active) goto no_render_data;
    if (actor == D_8009D254) {
        Entity_SetActionMode(actor, 0x15);
    } else {
        baseRows = (u8 *)D_800B161C - 0x784;
        row = (u32 *)(baseRows + actor->type_id * 0xC0);
        for (i = 0; i < 0x30; ++i) {
            if (row[i]) {
                Entity_SetActionMode(actor, i & 0xFFFF);
                break;
            }
        }
    }

    if (allocateFull) {
        allocation = Entity_AllocBlock(H((u8 *)actor->allocation_active, 0) * 8 + B((u8 *)actor->allocation_active, 3) * 12 + B((u8 *)actor->allocation_active, 2) * 32 + 0x50);
    } else {
        allocation = Entity_AllocBlock(B((u8 *)actor->allocation_active, 3) * 12 + B((u8 *)actor->allocation_active, 2) * 32);
        actor->flags |= 0x600000A0;
    }
    W(actor, 0x278) = (u32)allocation;

    if (scene[0] == 0) {
        areaType = D_800B0CE2;
        /* Keep the original address check distinct from constant folding. */
        asm volatile("" : "=r"(areaType) : "0"(areaType));
        if (areaType) goto render_special;
    }
    goto render_generic;
render_special: {
        Render_SetupEntityPrims((u8 *)actor + 0x1B4, (u8 *)actor->allocation_active, allocation + 0x50, 0x3C0,
                                0x100, 0, 0x1C0, 2, (u8 **)&renderSetup,
                                allocateFull);
    }
    goto render_done;
render_generic: {
        /* Keep archive reads on the generic path in the retail order. */
        asm volatile("" ::: "memory");
        archive = *D_800B0E64;
        directory = SceneAsset_ResolveOffset(archive, archive->directoryOffset);
        entry = directory->bankRootEntries;
        bankRows = SceneAsset_ResolveOffset(archive, entry & 0x3FFFFF);
        i = 0;
        if ((entry >> 22) == 0) goto scan_done;
        sceneType = scene[0];
        count = entry >> 22;
        walk = bankRows;
scan_loop:
        if (walk->source.bytes.id == sceneType) goto scan_done;
        ++i;
        if (i < count) { ++walk; goto scan_loop; }
scan_done:
        allocation = actor->allocation_block;
        recordWord = (volatile u32 *)((u32)(i * 12) + (u32)bankRows + 8);
        /* The retail code reads this word separately for three arguments. */
        value = *recordWord;
        Render_SetupEntityPrims((u8 *)actor + 0x1B4, (u8 *)actor->allocation_active, allocation + 0x50,
                                (value >> 6) & 0x3C0, (value >> 9) & 0x180,
                                0, ((*recordWord >> 18) & 0xFF) + 0x1C0,
                                (*recordWord >> 8) & 0xF, (u8 **)&renderSetup,
                                allocateFull);
    }
render_done:
    if (actor->action_data) {
        B(actor, 0x23C) = 0x80;
        B(actor, 0x23D) = 0xC;
        B(actor, 0x23E) = 0x18;
        Render_InitRoomPrimState((u8 *)actor + 0x1B4);
        Render_DrawWithAnim((u8 *)actor + 0x1B4, actor->action_data,
                            (s16)H(actor, 0x16), D_800BEA40, D_800B89F8);
        H(W(actor, 0x1B4), 0x14) = *(s16 *)((u8 *)actor + 0x224) * 2;
    }
    goto finish;
no_render_data:
    actor->flags |= 0xE0;
finish:
    return actor;
}
