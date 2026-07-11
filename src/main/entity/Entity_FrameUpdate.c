typedef signed char s8;
typedef signed short s16;
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef int s32;

#define U8_AT(ptr, off) (*(u8 *)((u8 *)(ptr) + (off)))
#define S16_AT(ptr, off) (*(s16 *)((u8 *)(ptr) + (off)))
#define U16_AT(ptr, off) (*(u16 *)((u8 *)(ptr) + (off)))
#define U32_AT(ptr, off) (*(u32 *)((u8 *)(ptr) + (off)))
#define S32_AT(ptr, off) (*(s32 *)((u8 *)(ptr) + (off)))
#define PTR_AT(ptr, off) (*(u8 **)((u8 *)(ptr) + (off)))

extern u32 D_8009D1A0;
extern u32 D_8009D1F4;
extern u8 *D_8009D20C;
extern u8 *D_8009D254;
extern s16 D_8009D2A4;
extern u8 *D_8009D278;
extern u32 D_8009DFB0[];
extern u8 D_800A76D8[];
extern u32 g_GameState[];
extern s16 D_800BCFFE;
extern s32 D_800BCFA4[];

void Battle_Update(void);
void Inventory_OpenAyaItemList(int arg0);
void Render_BeginSceneLoad(void);
int Menu_RunFrameWithArg(void *arg0);
void Render_SetViewport(void *arg0);
void Render_SetGteScreenOffset(void);
void Render_ResetGteScreenOffset(void);
void Entity_CopyParentPosition(void);
void Entity_CollectGarbage(void);
void Scene_UpdateEntityPositions(void);
void Render_InitRoomPrimState(u8 *actor);
int Anim_BuildRotationMatrices(u8 *actor, u8 *matrix_list, s16 arg2, int arg3);
void Render_TransformVertices(u8 *actor, s32 *view_matrix);
void Render_TransformSkinnedVertices(u8 *actor, s32 *view_matrix);
void Render_TransformMorphVertices(u8 *actor, s32 *view_matrix);
void Render_DrawEntity(u8 *actor, s32 *view_matrix);
void Render_DrawRoom(u8 *actor);
void Scene_LoadEntityTextures(void);
void Entity_AdvanceAnim(u8 *entity);

void func_80012774(void);
void func_8001A9F8(void);
void func_8005C498(void);
void func_80065E48(void *pos);
void func_80069660(void);
void Pm_RunPrimary(void);

static void Entity_FrameLoadRotMatrix(s32 *matrix) {
    int w0;
    int w1;
    int w2;
    int w3;
    int w4;

    w0 = matrix[0];
    w1 = matrix[1];
    w2 = matrix[2];
    w3 = matrix[3];
    w4 = matrix[4];

    asm volatile("ctc2 %0,$0" : : "r"(w0));
    asm volatile("ctc2 %0,$1" : : "r"(w1));
    asm volatile("ctc2 %0,$2" : : "r"(w2));
    asm volatile("ctc2 %0,$3" : : "r"(w3));
    asm volatile("ctc2 %0,$4" : : "r"(w4));
}

static void Entity_FrameLoadTrans(s32 *matrix) {
    int w5;
    int w6;
    int w7;

    w5 = matrix[5];
    w6 = matrix[6];
    w7 = matrix[7];

    asm volatile("ctc2 %0,$5" : : "r"(w5));
    asm volatile("ctc2 %0,$6" : : "r"(w6));
    asm volatile("ctc2 %0,$7" : : "r"(w7));
}

static void Entity_FrameTransformAxis(u16 *src, s16 *dst) {
    int x;
    int y;
    int z;

    asm volatile("mtc2 %0,$9" : : "r"(src[0]));
    asm volatile("mtc2 %0,$10" : : "r"(src[3]));
    asm volatile("mtc2 %0,$11" : : "r"(src[6]));
    asm volatile("nop");
    asm volatile(".word 0x4A49E012");
    asm volatile("mfc2 %0,$9" : "=r"(x));
    asm volatile("mfc2 %0,$10" : "=r"(y));
    asm volatile("mfc2 %0,$11" : "=r"(z));

    dst[0] = x;
    dst[3] = y;
    dst[6] = z;
}

static void Entity_FrameTransformZero(s32 *dst) {
    s32 zero_vec[3];

    zero_vec[0] = 0;
    zero_vec[1] = 0;
    zero_vec[2] = 0;
    asm volatile("lwc2 $0,0(%0)\n\t"
                 "lwc2 $1,4(%0)\n\t"
                 "nop\n\t"
                 "nop\n\t"
                 ".word 0x4A480012\n\t"
                 "swc2 $9,0(%1)\n\t"
                 "swc2 $10,4(%1)\n\t"
                 "swc2 $11,8(%1)"
                 :
                 : "r"(zero_vec), "r"(dst)
                 : "memory");
}

static void Entity_FrameBuildActorMatrix(u8 *entity) {
    s16 scale_matrix[16];
    s32 *matrix;
    u16 scale;

    matrix = (s32 *)(entity + 0x1E8);
    scale = U16_AT(entity, 0x26);

    S32_AT(entity, 0x1FC) = S16_AT(entity, 0x2A);
    S32_AT(entity, 0x200) = S16_AT(entity, 0x2E);
    S32_AT(entity, 0x204) = S16_AT(entity, 0x32);
    U16_AT(entity, 0x1E0) = U16_AT(entity, 0x38);
    U16_AT(entity, 0x1E2) = U16_AT(entity, 0x3A);
    U16_AT(entity, 0x1E4) = U16_AT(entity, 0x3C);
    Anim_BuildRotationMatrices(entity + 0x1E0, entity + 0x1E8, 0, 0);

    scale_matrix[0] = scale;
    scale_matrix[1] = 0;
    scale_matrix[2] = 0;
    scale_matrix[3] = 0;
    scale_matrix[4] = scale;
    scale_matrix[5] = 0;
    scale_matrix[6] = 0;
    scale_matrix[7] = 0;
    scale_matrix[8] = scale;

    Entity_FrameLoadRotMatrix(matrix);
    Entity_FrameTransformAxis((u16 *)scale_matrix, (s16 *)matrix);
    Entity_FrameTransformAxis((u16 *)scale_matrix + 1, (s16 *)matrix + 1);
    Entity_FrameTransformAxis((u16 *)scale_matrix + 2, (s16 *)matrix + 2);
    Entity_FrameLoadTrans(matrix);
    Entity_FrameTransformZero((s32 *)(entity + 0x1FC));
}

static void Entity_FrameRunPreCallbacks(void) {
    u8 *entity;
    void (*callback)(u8 *);

    entity = D_8009D20C;
    while (entity != 0) {
        callback = *(void (**)(u8 *))(entity + 0x190);
        if (callback != 0) {
            callback(entity);
        }
        entity = PTR_AT(entity, 4);
    }
}

static void Entity_FrameRebuildMatrices(void) {
    u8 *entity;

    entity = D_8009D20C;
    while (entity != 0) {
        if ((U32_AT(entity, 0x98) & 0x10040) == 0) {
            Entity_FrameBuildActorMatrix(entity);
        }
        entity = PTR_AT(entity, 4);
    }
}

static void Entity_FrameTransformAndDraw(void) {
    u8 *entity;
    u8 *actor;
    u32 flags;

    entity = D_8009D20C;
    while (entity != 0) {
        if (entity == D_8009D254 && (g_GameState[0] & 0x40000)) {
            entity = PTR_AT(entity, 4);
            continue;
        }

        if (PTR_AT(entity, 0x1AC) != 0) {
            U16_AT(PTR_AT(entity, 0x1B4), 0x16) = U16_AT(entity, 0x26);
        }

        flags = U32_AT(entity, 0x98);
        if (flags & 0x40) {
            if (flags & 0x2000) {
                Render_TransformSkinnedVertices(entity + 0x1B4, D_800BCFA4);
                S32_AT(entity, 0x28) = S16_AT(entity, 0x254) << 16;
                S32_AT(entity, 0x30) = S16_AT(entity, 0x258) << 16;
                S32_AT(entity, 0x2C) = S16_AT(entity, 0x256) << 16;
            } else if (PTR_AT(entity, 0x1AC) == 0) {
                S16_AT(entity, 0x228) = S16_AT(entity, 0x2A);
                S16_AT(entity, 0x22A) = S16_AT(entity, 0x2E);
                S16_AT(entity, 0x22C) = S16_AT(entity, 0x32);
            }
        } else {
            actor = entity + 0x1B4;
            Render_InitRoomPrimState(actor);
            if (PTR_AT(entity, 0x1B0) != 0) {
                Anim_BuildRotationMatrices(actor, PTR_AT(entity, 0x1B0), S16_AT(entity, 0x16), 1);
            }
            Render_TransformVertices(actor, 0);
            Render_TransformSkinnedVertices(actor, D_800BCFA4);
            if (!(U32_AT(entity, 0x98) & 0x20000000)) {
                Render_TransformMorphVertices(actor, D_800BCFA4);
                Render_DrawEntity(actor, D_800BCFA4);
                if (U32_AT(entity, 0x98) & 0x8800) {
                    U16_AT(entity, 0x250) |= 1;
                }
                Render_DrawRoom(entity);
            }
        }
        entity = PTR_AT(entity, 4);
    }
}

static void Entity_FrameDispatchActiveEntities(void) {
    u8 *entity;

    if (D_8009D1A0 & 0x100) {
        if (!(g_GameState[0] & 0x40000) && D_8009D254 != 0) {
            Entity_AdvanceAnim(D_8009D254);
        }
        return;
    }

    entity = D_8009D20C;
    while (entity != 0) {
        if ((entity != D_8009D254 || !(g_GameState[0] & 0x40000)) &&
            (U32_AT(entity, 0x98) & 0x800040) == 0) {
            Entity_AdvanceAnim(entity);
        }
        entity = PTR_AT(entity, 4);
    }
}

void Entity_FrameUpdate(void) {
    u8 *entity;
    s32 camera[3];

    if (!(D_8009D1A0 & 4)) {
        Entity_FrameRunPreCallbacks();
    }

    if (g_GameState[0] & 0x200) {
        goto tail_updates;
    }

    if (D_8009D1A0 & 2) {
        Battle_Update();
        goto tail_updates;
    }

    if (D_8009D254 != 0 && PTR_AT(D_8009D254, 0) != 0 && (D_8009D1F4 & 0x80) &&
        !(D_8009D1A0 & 0x2000) && !(g_GameState[0] & 0x3400)) {
        Inventory_OpenAyaItemList(0);
        Render_BeginSceneLoad();
        D_8009D1A0 |= 4;
        g_GameState[0] |= 0x9000;
    }

    D_8009D2A4 = Menu_RunFrameWithArg(D_800A76D8);
    if (D_8009D2A4 != 0) {
        Render_BeginSceneLoad();
        D_8009D1A0 &= ~4U;
        g_GameState[0] &= 0xFFFF6FFF;
    }

    func_80069660();

    if (!(D_8009D1A0 & 4)) {
        func_8001A9F8();
    }

    Entity_CopyParentPosition();
    Scene_IsNotBattleMode();

    entity = D_8009D254;
    if (entity != 0) {
        camera[0] = S32_AT(entity, 0x28);
        camera[1] = S32_AT(entity, 0x2C) - ((s32)D_800BCFFE << 16);
        camera[2] = S32_AT(entity, 0x30);
    } else {
        camera[0] = 0;
        camera[1] = 0;
        camera[2] = 0;
        entity = D_8009D20C;
        while (entity != 0) {
            if (PTR_AT(entity, 0x1AC) != 0) {
                camera[0] = S32_AT(entity, 0x28);
                camera[1] = S32_AT(entity, 0x2C) - ((s32)D_800BCFFE << 16);
                camera[2] = S32_AT(entity, 0x30);
                break;
            }
            entity = PTR_AT(entity, 4);
        }
    }

    Render_SetViewport(camera);

    if (!(D_8009D1A0 & 4)) {
        Render_SetGteScreenOffset();
        Entity_FrameRebuildMatrices();
    }

    Render_SetGteScreenOffset();
    Entity_FrameTransformAndDraw();

    Scene_LoadEntityTextures();
    Pm_RunPrimary();
    Render_ResetGteScreenOffset();

    if (!(D_8009D1A0 & 4)) {
        Entity_FrameDispatchActiveEntities();
    }

tail_updates:
    if (!(D_8009D1A0 & 4)) {
        Scene_UpdateEntityPositions();
        func_80012774();
        Entity_CollectGarbage();
        entity = D_8009D20C;
        while (entity != 0) {
            U32_AT(entity, 0x98) &= 0xEFFFFFFF;
            entity = PTR_AT(entity, 4);
        }
    }
}
