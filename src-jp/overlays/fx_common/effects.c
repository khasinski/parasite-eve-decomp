#include "common.h"
#include "pe1/script_queue.h"

#include "fx_common.h"
#include "fx_common_motion.h"
#include "fx_common_render.h"

void FxCommon_MuteMusic(void)
{
    Akao_Cmd_A9(1, 0);
}

void FxCommon_FadeMusicOut(void)
{
    Akao_Cmd_A9(30, 0);
}

/* Choose the room to return to once the effect sequence ends, clear the
 * screen, fade the music out and hand control back to the scene loader. */
void FxCommon_EndSequence(void)
{
    FxCommonRect area;
    s32 room;
    u32 night;
    u32 band;
    u8 fade;
    int i;

    fade = 1;
    night = g_ScriptVariableBank[SCRIPT_VAR_FLAGS] & 0x2000;
    g_ScriptVariableBank[SCRIPT_VAR_ROOM] = 999;
    g_SceneDispatchToken = 0xA9400048;
    room = g_ScriptVariableBank[SCRIPT_VAR_AREA];
    if (room == 0x80) {
        g_SceneDispatchToken = 0xA80650C8;
    } else if (room == 0x78) {
        g_SceneDispatchToken = 0xA8003348;
        fade = 0;
    } else if (room == 0x208) {
        g_SceneDispatchToken = 0xA80032C8;
        fade = 0;
    } else if (room == 0x210) {
        if (night)
            g_SceneDispatchToken = 0xA8029148;
        else
            g_SceneDispatchToken = 0xA80290C8;
    } else {
        switch (g_FxCommonExitVariant) {
        case 0:
            g_SceneDispatchToken = 0xA80281C8;
            break;
        case 1:
            g_SceneDispatchToken = 0xA80260C8;
            break;
        case 2:
            g_SceneDispatchToken = 0xA8009348;
            break;
        case 3:
            g_SceneDispatchToken = 0xA8046048;
            break;
        case 4:
            if (room == 0x1C0) {
                g_SceneDispatchToken = 0xA80034C8;
                fade = 0;
            } else if (night) {
                g_SceneDispatchToken = 0xA80222C8;
            } else {
                g_SceneDispatchToken = 0xA80630C8;
            }
            break;
        case 5:
            g_SceneDispatchToken = 0xA8049048;
            break;
        case 6:
            g_SceneDispatchToken = 0xA8002248;
            break;
        case 7:
            if (room == 0xD0) {
                g_SceneDispatchToken = 0xA80033C8;
                fade = 0;
            } else if (room == 0x178) {
                g_SceneDispatchToken = 0xA80033C8;
                fade = 0;
            } else {
                band = room - 0x180;
                if (band < 0x88)
                    g_SceneDispatchToken = 0xA80201C8;
                else
                    g_SceneDispatchToken = 0xA8004348;
            }
            break;
        case 8:
            if (room == 0xE0) {
                g_SceneDispatchToken = 0xA80034C8;
                fade = 0;
            } else if (room >= 0x128) {
                g_SceneDispatchToken = 0xA8005448;
            } else {
                g_SceneDispatchToken = 0xA8067248;
            }
            break;
        case 9:
            if (room == 0xB8 || room == 0x148) {
                g_SceneDispatchToken = 0xA8003448;
                fade = 0;
            } else if (night) {
                g_SceneDispatchToken = 0xA8029148;
            } else {
                g_SceneDispatchToken = 0xA80290C8;
            }
            break;
        }
    }

    SetDispMask(0);
    ResetGraph(1);
    area.x = 0;
    area.y = 0;
    area.w = 0x140;
    area.h = 0x1C0;
    ClearImage(&area, 0, 0, 0);
    DrawSync(0);
    PutDispEnv(g_RenderDispEnvArray);
    if (fade == 1)
        Akao_Cmd_C1_WithSlot(g_SceneAudioSlot0Handle, 0x1E, 0);
    Akao_Cmd_A9(0x1E, 0);
    for (i = 0; i < 0x1E; i++)
        VSync(0);
    if (fade == 1) {
        g_SceneAudioSlot0Key = -1;
        g_GameState.scene_audio.tracks.banks[0] = -1;
        g_GameStateFlagsWord &= ~0x40;
        Akao_Cmd_F0();
    }
    Akao_Cmd_F1();
    Render_ConsumeFontShuffleSeed();
}

/* The effect overlay's own frame loop: run every effect system, compact the
 * ordering table, present the frame and flip the double buffer until the
 * scene asks to leave. */
void FxCommon_RunFrameLoop(void)
{
    FxCommonBuffer *next;
    int last;
    int handle;
    u8 started;
    int scanning;
    int i;
    FxCommonAddress current;

    g_ActiveDrawSlot = 0;
    g_FxCommonResetRequested = 0;
    last = 0;
    FxCommon_InitScene();
    FxCommon_SetAkao3dVolumeRange(g_FxCommonViewMode != 10);
    srand(VSync(-1));
    started = 0;
    g_FxCommonFlyerPathPosition = rand() % 3000000;
    handle = 0;
    if (g_FxCommonViewMode == 10)
        g_FxCommonFlyerPathPosition = 120000;

    while (g_FxCommonExitRequested == 0) {
        if (g_FxCommonViewMode != 10)
            Field_HandleStateTransition();
        FxCommon_StepEffects();
        if (g_FxCommonResetRequested != 0)
            continue;

        FxCommon_UpdateCameraMatrices();
        FxCommon_UpdateFrustumPlanes((void *)&g_FxCommonCameraPosition);
        if (g_FxCommonScreenFade != 0)
            g_FxCommonScreenFade = FxCommon_StepFadeIntensity(g_FxCommonScreenFade);
        FxCommon_DrawNightShadeAndMarkers();
        FxCommon_DrawSceneNodes();
        FxCommon_StepPathNodes();
        if (!started) {
            handle = FxCommon_SendTableCommand3d(0xABE, g_FxCommonPathNodes[0]->matrix.t);
            started = 1;
        } else if (g_FxCommonDemoExiting == 0) {
            FxCommon_Apply3dPan(handle, g_FxCommonPathNodes[0]->matrix.t);
        }
        if (g_FxCommonViewMode == 10 || (g_FxCommonPlaying == 1 && g_FxCommonPlayAdvance == 0))
            Menu_DrawTextboxEntries();

        scanning = 1;
        for (i = 0xFFF; i >= 0; i--) {
            if (scanning) {
                if ((FxCommonPacketTag *)(g_FxCommonActiveBuffer->allocation[i].packed |
                                          0x80000000) ==
                    &g_FxCommonActiveBuffer->allocation[i - 1]) {
                    last = i;
                    scanning = 0;
                }
            } else if ((FxCommonPacketTag *)(g_FxCommonActiveBuffer->allocation[i].packed |
                                             0x80000000) !=
                       &g_FxCommonActiveBuffer->allocation[i - 1]) {
                current.pointer = &g_FxCommonActiveBuffer->allocation[i];
                g_FxCommonActiveBuffer->allocation[last].packed = current.word & 0xFFFFFF;
                scanning = 1;
            }
        }

        g_FxCommonHsyncCount = VSync(1);
        DrawSync(0);
        VSync(2);
        FxCommon_StepMotionBlend();
        ResetGraph(1);
        PutDrawEnv(((FxCommonFrame *)g_FxCommonActiveBuffer)->drawEnv);
        PutDispEnv(((FxCommonFrame *)g_FxCommonActiveBuffer)->dispEnv);
        DrawOTag(&g_FxCommonActiveBuffer->allocation[0xFFF]);
        next = &g_FxCommonFrames[0].buffer;
        if (g_FxCommonActiveBuffer == next)
            next = &g_FxCommonFrames[1].buffer;
        g_FxCommonActiveBuffer = next;
        g_ActiveDrawSlot ^= 1;
        ClearOTagR(next->allocation, 0x1000);
        FxCommon_SelectBufferAllocation((void **)g_FxCommonActiveBuffer);
    }
    if (g_FxCommonResetRequested == 0)
        FxCommon_EndSequence();
}

void FxCommon_DrawNightShadeAndMarkers(void)
{
    if (g_FxCommonNight == 1) {
        u32 *dst = (u32 *)g_FxCommonShadeQuads;
        FxCommonBuffer *owner = g_FxCommonActiveBuffer;
        FxCommonBuffer *sentinel = &g_FxCommonFrames[0].buffer;
        u32 lowMask;
        u32 *base;
        u32 highMask;
        u32 *src1;
        u32 *src2;
        u32 destWord;
        u32 lowWord;
        FxCommonAddress address;

        if (owner != sentinel)
            dst += 9;
        lowMask = 0xFFFFFF;
        base = (u32 *)g_FxCommonShadeQuads;
        highMask = 0xFF000000;
        src1 = (u32 *)(owner)->allocation;
        destWord = *dst;
        lowWord = src1[0xFFF];
        destWord &= highMask;
        lowWord &= lowMask;
        destWord |= lowWord;
        *dst = destWord;
        src2 = (u32 *)(owner)->allocation;
        if (owner != sentinel)
            base += 9;
        address.pointer = base;
        lowWord = address.word & lowMask;
        src2[0xFFF] = (src2[0xFFF] & highMask) | lowWord;
    }
    if (g_FxCommonMarkerFade != 0)
        g_FxCommonMarkerFade = FxCommon_DrawEffectMarkers(g_FxCommonMarkerFade);
}

#define DRAW(node, pass, force, mirrored) \
    FxCommon_DrawTransformNodeCulled((node), g_FxCommonActiveBuffer, (pass), (force), (mirrored))
#define DRAW_PAIR(node, pass, mode) \
    FxCommon_DrawLodNode((node), g_FxCommonActiveBuffer, (pass), (mode))

/* Draw every scene node for this frame, each with its depth bias: the
 * backdrop, the turning fans, the static props, the camera path nodes and
 * their echoes, the effect node pairs and the overlay nodes. */
void FxCommon_DrawSceneNodes(void)
{
    int i;

    if (g_FxCommonViewMode != 0) {
        g_FxCommonDepthBias = 0xFA1;
        DRAW(g_FxCommonType54Node, 1, 1, 0);
    }
    g_FxCommonDepthBias = 0xFA0;
    DRAW(g_FxCommonRuntime.node25, 1, 1, 0);
    g_FxCommonDepthBias = 0xF9F;
    DRAW(g_FxCommonType26Nodes[0], 1, 1, 0);
    DRAW(g_FxCommonType26Nodes[1], 1, 1, 0);
    DRAW(g_FxCommonType26Nodes[2], 1, 1, 0);
    DRAW(g_FxCommonType26Nodes[3], 1, 1, 0);
    g_FxCommonType26Nodes[0]->state[0] += 10;
    g_FxCommonType26Nodes[1]->state[0] += 2;
    g_FxCommonType26Nodes[2]->state[0] += 40;
    g_FxCommonType26Nodes[3]->state[0] += 30;
    if (g_FxCommonType26Nodes[0]->state[0] > 0x6EE0)
        g_FxCommonType26Nodes[0]->state[0] = -0x6328;
    if (g_FxCommonType26Nodes[1]->state[0] > 0x6EE0)
        g_FxCommonType26Nodes[1]->state[0] = -0x6328;
    if (g_FxCommonType26Nodes[2]->state[0] > 0x6EE0)
        g_FxCommonType26Nodes[2]->state[0] = -0x6328;
    if (g_FxCommonType26Nodes[3]->state[0] > 0x6EE0)
        g_FxCommonType26Nodes[3]->state[0] = -0x6328;

    g_FxCommonDepthBias = 1000;
    DRAW(g_FxCommonSceneNodes[1], 0, 1, 0);
    DRAW(g_FxCommonSceneNodes[3], 0, 1, 0);
    g_FxCommonDepthBias = 0xF3B;
    DRAW(g_FxCommonSceneNodes[0], 1, 1, 0);
    DRAW(g_FxCommonSceneNodes[2], 1, 1, 0);
    g_FxCommonDepthBias = 0xF3A;
    DRAW(g_FxCommonSceneNodes[4], 1, 1, 0);
    g_FxCommonDepthBias = 0xF39;
    DRAW(g_FxCommonSceneNodes[5], 1, 1, 0);
    g_FxCommonDepthBias = 0;
    DRAW(g_FxCommonSceneNodes[6], 0, 1, 0);
    DRAW(g_FxCommonSceneNodes[7], 0, 1, 0);
    DRAW(g_FxCommonSceneNodes[8], 0, 1, 0);
    DRAW(g_FxCommonSceneNodes[9], 0, 1, 0);
    DRAW(g_FxCommonSceneNodes[10], 0, 1, 0);
    DRAW(g_FxCommonType51Node, 0, 1, 0);

    if (g_FxCommonPathNodes[0]->kind == 0x24) {
        g_FxCommonPathNodes[0]->kind = 0x52;
        g_FxCommonPathNodes[1]->kind = 0x52;
        g_FxCommonPathNodes[2]->kind = 0x52;
        g_FxCommonPathNodes[3]->kind = 0x52;
    } else {
        g_FxCommonPathNodes[0]->kind = 0x24;
        g_FxCommonPathNodes[1]->kind = 0x24;
        g_FxCommonPathNodes[2]->kind = 0x24;
        g_FxCommonPathNodes[3]->kind = 0x24;
    }
    g_FxCommonDepthBias = 0;
    DRAW(g_FxCommonPathNodes[0], 0, 0, 0);
    if (g_FxCommonViewMode != 2)
        DRAW(g_FxCommonPathNodes[2], 0, 0, 0);
    g_FxCommonDepthBias = 1000;
    DRAW(g_FxCommonPathNodes[1], 0, 0, 1);
    if (g_FxCommonViewMode != 2)
        DRAW(g_FxCommonPathNodes[3], 0, 0, 1);

    g_FxCommonDepthBias = 0;
    DRAW_PAIR(g_FxCommonLodNodes[0], 1, 0);
    DRAW_PAIR(g_FxCommonPairedNodes[0], 1, 0);
    g_FxCommonDepthBias = 1000;
    DRAW_PAIR(g_FxCommonLodNodes[1], 1, 2);
    DRAW_PAIR(g_FxCommonPairedNodes[1], 1, 2);

    g_FxCommonDepthBias = 0;
    DRAW(g_FxCommonPathTailNodes[0], 0, 0, 0);
    DRAW(g_FxCommonPathTailNodes[1], 0, 0, 0);
    g_FxCommonDepthBias = 10;
    DRAW(g_FxCommonPathTailNodes[0], 0, 0, 1);
    DRAW(g_FxCommonPathTailNodes[1], 0, 0, 1);
    g_FxCommonDepthBias = 0;
    DRAW(g_FxCommonType21Nodes[0], 0, 1, 0);
    g_FxCommonDepthBias = 800;
    DRAW(g_FxCommonType21Nodes[1], 0, 1, 1);

    if (g_FxCommonNight == 1) {
        g_FxCommonNightRandomAccumWriteOnly[0] += (s16)(rand() % 40 + 20);
        g_FxCommonNightRandomAccumWriteOnly[1] += (s16)(rand() % 52 + 26);
        g_FxCommonEchoNodes[0]->matrix = g_FxCommonPathNodes[0]->matrix;
        g_FxCommonEchoNodes[0]->seed = g_FxCommonPathNodes[0]->seed;
        g_FxCommonEchoNodes[0]->kind = 0x53;
        g_FxCommonEchoNodes[2]->matrix = g_FxCommonPathNodes[2]->matrix;
        g_FxCommonEchoNodes[2]->seed = g_FxCommonPathNodes[2]->seed;
        g_FxCommonEchoNodes[2]->kind = 0x53;
        g_FxCommonEchoNodes[1]->matrix = g_FxCommonPathNodes[0]->matrix;
        g_FxCommonEchoNodes[1]->seed = g_FxCommonPathNodes[0]->seed;
        g_FxCommonEchoNodes[1]->kind = 0x53;
        g_FxCommonEchoNodes[3]->matrix = g_FxCommonPathNodes[2]->matrix;
        g_FxCommonEchoNodes[3]->seed = g_FxCommonPathNodes[2]->seed;
        g_FxCommonEchoNodes[3]->kind = 0x53;
        g_FxCommonDepthBias = 0;
        DRAW(g_FxCommonEchoNodes[0], 0, 0, 0);
        g_FxCommonDepthBias = 800;
        DRAW(g_FxCommonEchoNodes[1], 0, 0, 1);
    }

    for (i = 0; i < g_FxCommonRouteNodeCount >> 1; i++) {
        g_FxCommonDepthBias = 0;
        if (g_ScriptVariableBank[SCRIPT_VAR_FLAGS] & 0x2000) {
            g_FxCommonFlatColor[0] = 60;
            g_FxCommonFlatColor[1] = 60;
            g_FxCommonFlatColor[2] = 40;
        } else {
            g_FxCommonFlatColor[0] = 20;
            g_FxCommonFlatColor[1] = 40;
            g_FxCommonFlatColor[2] = 40;
        }
        DRAW_PAIR(g_FxCommonRouteNodes[i * 2], 0, 0);
        if (g_FxCommonDrawTintedCopies == 1 && (i & 3)) {
            g_FxCommonDepthBias = 800;
            DRAW_PAIR(g_FxCommonRouteNodes[i * 2 + 1], 0, 1);
        }
    }

    g_FxCommonDepthBias = 0;
    for (i = 0; i < 8; i++)
        DRAW_PAIR(g_FxCommonRuntime.pairedNodes[i], 0, 0);
    g_FxCommonDepthBias = 0x23A;
    DRAW_PAIR(g_FxCommonRuntime.pairedNodes[8], 0, 0);
    g_FxCommonDepthBias = 0;
    DRAW_PAIR(g_FxCommonRuntime.pairedNodes[9], 0, 0);
    if (g_FxCommonDrawTintedCopies == 1) {
        g_FxCommonDepthBias = 800;
        if (g_FxCommonPlaying) {
            DRAW_PAIR(g_FxCommonRuntime.pairedNodes[11], 0, 1);
            DRAW_PAIR(g_FxCommonRuntime.pairedNodes[12], 0, 1);
            DRAW_PAIR(g_FxCommonRuntime.pairedNodes[13], 0, 1);
            DRAW_PAIR(g_FxCommonRuntime.pairedNodes[14], 0, 1);
            DRAW_PAIR(g_FxCommonRuntime.pairedNodes[16], 0, 1);
            DRAW_PAIR(g_FxCommonRuntime.pairedNodes[17], 0, 1);
            DRAW_PAIR(g_FxCommonRuntime.pairedNodes[18], 0, 1);
            DRAW_PAIR(g_FxCommonRuntime.pairedNodes[19], 0, 1);
        }
        DRAW_PAIR(g_FxCommonRuntime.pairedNodes[15], 0, 1);
    }
    g_FxCommonDepthBase = 500;
    FxCommon_DrawTransformNode(g_FxCommonRuntime.node0, g_FxCommonActiveBuffer);
}

#undef DRAW
#undef DRAW_PAIR

/* Move the path-following nodes one step along their recorded paths, bank
 * the two flyers into their turns, and copy each flyer onto its shadow. */
void FxCommon_StepPathNodes(void)
{
    GteShortVector bank0;
    GteShortVector bank1;

    FxCommon_SamplePath(g_FxCommonTailPathPosition, 0x4A, Draw_GetTimTableEntry(&g_FxCommonTimTable, 2),
                  (FxCommonVec3 *)g_FxCommonPathNodes[4]->matrix.t,
                  &g_FxCommonPathNodes[4]->seed.angles);
    FxCommon_SamplePath(g_FxCommonTailPathPosition, 0x4B, Draw_GetTimTableEntry(&g_FxCommonTimTable, 2),
                  (FxCommonVec3 *)g_FxCommonPathNodes[5]->matrix.t,
                  &g_FxCommonPathNodes[5]->seed.angles);
    FxCommon_SamplePath(g_FxCommonFlyerPathPosition, 0x4C, Draw_GetTimTableEntry(&g_FxCommonTimTable, 2),
                  (FxCommonVec3 *)g_FxCommonPathNodes[0]->matrix.t, &bank0);
    FxCommon_SamplePath(g_FxCommonFlyerPathPosition, 0x4D, Draw_GetTimTableEntry(&g_FxCommonTimTable, 2),
                  (FxCommonVec3 *)g_FxCommonPathNodes[2]->matrix.t, &bank1);

    g_FxCommonPathNodes[0]->seed.component[0] = bank0.x;
    g_FxCommonPathNodes[0]->seed.component[1] = bank0.y;
    g_FxCommonPathNodes[0]->seed.component[2] += bank0.z;
    if (g_FxCommonPathNodes[0]->seed.component[2] > 0x200)
        g_FxCommonPathNodes[0]->seed.component[2] = 0x200;
    if (g_FxCommonPathNodes[0]->seed.component[2] < -0x200)
        g_FxCommonPathNodes[0]->seed.component[2] = -0x200;
    if (g_FxCommonPathNodes[0]->seed.component[2] > 0)
        g_FxCommonPathNodes[0]->seed.component[2] -= 8;
    if (g_FxCommonPathNodes[0]->seed.component[2] < 0)
        g_FxCommonPathNodes[0]->seed.component[2] += 8;

    g_FxCommonPathNodes[2]->seed.component[0] = bank1.x;
    g_FxCommonPathNodes[2]->seed.component[1] = bank1.y;
    g_FxCommonPathNodes[2]->seed.component[2] += bank1.z;
    if (g_FxCommonPathNodes[2]->seed.component[2] > 0x180)
        g_FxCommonPathNodes[2]->seed.component[2] = 0x180;
    if (g_FxCommonPathNodes[2]->seed.component[2] < -0x180)
        g_FxCommonPathNodes[2]->seed.component[2] = -0x180;
    if (g_FxCommonPathNodes[2]->seed.component[2] > 0)
        g_FxCommonPathNodes[0]->seed.component[2] -= 8;
    if (g_FxCommonPathNodes[2]->seed.component[2] < 0)
        g_FxCommonPathNodes[0]->seed.component[2] += 8;

    g_FxCommonPathNodes[4]->seed.component[1] -= 0x400;
    g_FxCommonPathNodes[5]->seed.component[1] -= 0x400;
    g_FxCommonPathNodes[0]->seed.component[1] -= 0x400;
    g_FxCommonPathNodes[2]->seed.component[1] -= 0x400;

    g_FxCommonPathNodes[1]->matrix = g_FxCommonPathNodes[0]->matrix;
    g_FxCommonPathNodes[1]->seed = g_FxCommonPathNodes[0]->seed;
    g_FxCommonPathNodes[3]->matrix = g_FxCommonPathNodes[2]->matrix;
    g_FxCommonPathNodes[3]->seed = g_FxCommonPathNodes[2]->seed;
    g_FxCommonTailPathPosition += 4;
    g_FxCommonFlyerPathPosition += 0x10;
}

void FxCommon_SnapMotionWhole(s16 base)
{
    FxCommonMotionRecord sample;
    GteShortVector extra;
    unsigned int i;

    g_FxCommonMotionBlendFrames = 0;
    for (i = 0; i < 10; i++) {
        FxCommon_SamplePath(i << 8, base + 0x40,
                      Draw_GetTimTableEntry(&g_FxCommonTimTable, 2), &sample.vector, &extra);
        g_FxCommonMotionPositions[i].vector.x = sample.vector.x << 16;
        g_FxCommonMotionPositions[i].vector.y = sample.vector.y << 16;
        g_FxCommonMotionPositions[i].vector.z = sample.vector.z << 16;
    }
}

void FxCommon_BeginMotionBlend(s16 base)
{
    FxCommonMotionRecord sample;
    GteShortVector extra;
    unsigned int i;

    g_FxCommonMotionBlendFrames = 0x20;
    for (i = 0; i < 10; i++) {
        FxCommon_SamplePath(i << 8, base + 0x40,
                      Draw_GetTimTableEntry(&g_FxCommonTimTable, 2), &sample.vector, &extra);
        g_FxCommonMotionDeltas[i].vector.x =
            ((sample.vector.x << 16) - g_FxCommonMotionPositions[i].vector.x) >> 5;
        g_FxCommonMotionDeltas[i].vector.y =
            ((sample.vector.y << 16) - g_FxCommonMotionPositions[i].vector.y) >> 5;
        g_FxCommonMotionDeltas[i].vector.z =
            ((sample.vector.z << 16) - g_FxCommonMotionPositions[i].vector.z) >> 5;
    }
}

void FxCommon_StepMotionBlend(void)
{
    FxCommonMotionLane *x;
    FxCommonMotionLane *y;
    FxCommonMotionLane *z;
    FxCommonMotionLane *end;
    FxCommonMotionByte *base;
    int sourceOffset;

    if (g_FxCommonMotionBlendFrames <= 0) {
        return;
    }

    g_FxCommonMotionBlendFrames--;
    base = (FxCommonMotionByte *)g_FxCommonMotionPositionZ;
    z = g_FxCommonMotionPositionZ;
    y = (FxCommonMotionLane *)(base - 4);
    x = (FxCommonMotionLane *)(base - 8);
    sourceOffset = 0;
    end = (FxCommonMotionLane *)(base + 0xA0);
    do {
        x->value += *(s32 *)(g_FxCommonMotionDeltaX + sourceOffset);
        y->value += *(s32 *)(g_FxCommonMotionDeltaY + sourceOffset);
        z->value += *(s32 *)(g_FxCommonMotionDeltaZ + sourceOffset);
        x++;
        y++;
        z++;
        sourceOffset += 0x10;
    } while (z < end);
}

#define FX_MODE_PACKET                                          \
    (g_FxCommonActiveBuffer == &g_FxCommonFrames[0].buffer ? &g_FxCommonDrawModes1[1] \
                                               : &g_FxCommonDrawModes1[0])

/* Fade the effect markers in or out by eight steps and draw, for every
 * enabled effect, a line from its projected position to a two-part label
 * (the selected effect uses the brighter palette). */
s16 FxCommon_DrawEffectMarkers(s16 value)
{
    s16 level;
    FxCommonDrawModePacket *mode;
    FxCommonBuffer *buffer;
    FxCommonBuffer *leftBuffer;
    FxCommonBuffer *rightBuffer;
    FxCommonPacketTag *ot;
    FxCommonAddress address;
    FxCommonAddress lineLink;
    FxCommonAddress leftLink;
    FxCommonAddress rightLink;
    FxCommonLinePacket *line;
    FxCommonMarkerQuad *quad;
    GteShortVector rotation;
    GteShortVector position;
    GteShortVector unused;
    s32 screen0;
    s32 screen1;
    s32 screen2;
    s32 depth;
    s32 flag;
    u32 xy;
    u32 *vertex;
    u8 u;
    s16 x;
    s16 y;
    int i;

    level = 0;
    if (value > 0) {
        if (value < 0xFF)
            value += 8;
        if (value > 0xFF)
            value = 0xFF;
        level = value;
    }
    if (value < 0) {
        if (value >= -0xFE)
            value -= 8;
        if (value < -0xFF)
            value = -0xFF;
        level = value + 0xFF;
    }

    mode = FX_MODE_PACKET;
    mode->tag.bits.address = g_FxCommonActiveBuffer->allocation[10].bits.address;
    ot = &g_FxCommonActiveBuffer->allocation[10];
    address.pointer = FX_MODE_PACKET;
    ot->bits.address = address.word;

    for (i = 0; i < 10; i++) {
        position.x = g_FxCommonMotionWhole[i].x.whole;
        position.y = g_FxCommonMotionWhole[i].y.whole;
        position.z = g_FxCommonMotionWhole[i].z.whole;
        if (g_FxCommonEffectSetup[i].enabled != 1 || level == 0)
            continue;

        rotation.x = g_FxCommonEffectSetup[i].position[0];
        rotation.y = g_FxCommonEffectSetup[i].position[1];
        rotation.z = g_FxCommonEffectSetup[i].position[2];
        RotTransPers3(&rotation, &position, &unused, &screen0, &screen1,
                      &screen2, &depth, &flag);

        line = (FxCommonLinePacket *)g_FxCommonActiveBuffer->data;
        line->tag.bytes.length = 3;
        line->code = 0x40;
        SetSemiTrans(line, 1);
        vertex = &line->xy0;
        *vertex++ = screen0;
        *vertex = screen1;
        if (g_FxCommonRoomVariant == i) {
            line->r = level >> 1;
            line->g = level >> 1;
            line->b = level >> 1;
        } else {
            line->r = level >> 3;
            line->g = level >> 3;
            line->b = level >> 3;
        }
        buffer = g_FxCommonActiveBuffer;
        lineLink.pointer = line;
        line->tag.bits.address = buffer->allocation[10].bits.address;
        buffer->allocation[10].bits.address = lineLink.word;
        line++;
        buffer->data = (u8 *)line;
        quad = (FxCommonMarkerQuad *)buffer->data;

        xy = screen1;
        x = xy;
        y = xy >> 16;
        u = 0;
        SetPolyGT4(quad);
        SetSemiTrans(quad, 1);
        quad->r0 = level >> 1;
        quad->g0 = level >> 1;
        quad->b0 = level >> 1;
        quad->r1 = level >> 1;
        quad->g1 = level >> 1;
        quad->b1 = level >> 1;
        quad->r2 = level >> 1;
        quad->g2 = level >> 1;
        quad->b2 = level >> 1;
        quad->r3 = level >> 1;
        quad->g3 = level >> 1;
        quad->b3 = level >> 1;
        quad->x0 = x;
        quad->y0 = y;
        quad->x1 = x + 0x7F;
        quad->y1 = y;
        quad->x2 = x;
        quad->y2 = y + 12;
        quad->x3 = x + 0x7F;
        quad->y3 = y + 13;
        quad->u0 = u;
        quad->v0 = i * 13;
        quad->u1 = u + 0x7F;
        quad->v1 = i * 13;
        quad->u2 = u;
        quad->v2 = i * 13 + 12;
        quad->u3 = u + 0x7F;
        quad->v3 = i * 13 + 12;
        if (g_FxCommonRoomVariant == i)
            quad->clut = GetClut(0x3C0, 0x90);
        else
            quad->clut = GetClut(0x3C0, 0x8F);
        quad->tpage = GetTPage(0, 1, 0x3C0, 0);
        leftBuffer = g_FxCommonActiveBuffer;
        leftLink.pointer = quad;
        quad->tag.bits.address = leftBuffer->allocation[9].bits.address;
        leftBuffer->allocation[9].bits.address = leftLink.word;
        quad++;

        u = 0x80;
        SetPolyGT4(quad);
        SetSemiTrans(quad, 1);
        quad->r0 = u;
        quad->g0 = u;
        quad->b0 = u;
        quad->r1 = u;
        quad->g1 = u;
        quad->b1 = u;
        quad->r2 = u;
        quad->g2 = u;
        quad->b2 = u;
        quad->r3 = u;
        quad->g3 = u;
        quad->b3 = u;
        quad->x0 = x;
        quad->y0 = y;
        quad->x1 = x + 0x7F;
        quad->y1 = y;
        quad->x2 = x;
        quad->y2 = y + 12;
        quad->x3 = x + 0x7F;
        quad->y3 = y + 13;
        quad->u0 = u;
        quad->v0 = i * 13;
        quad->u1 = u + 0x7F;
        quad->v1 = i * 13;
        quad->u2 = u;
        quad->v2 = i * 13 + 12;
        quad->u3 = u + 0x7F;
        quad->v3 = i * 13 + 12;
        quad->clut = GetClut(0x3C0, 0x91);
        quad->tpage = GetTPage(0, 2, 0x3C0, 0);
        rightBuffer = g_FxCommonActiveBuffer;
        rightLink.pointer = quad;
        quad->tag.bits.address = rightBuffer->allocation[9].bits.address;
        rightBuffer->allocation[9].bits.address = rightLink.word;
        quad++;
        rightBuffer->data = (u8 *)quad;
    }
    return value;
}

#undef FX_MODE_PACKET

s16 FxCommon_StepFadeIntensity(s16 value)
{
    s16 result;

    result = value;
    if (value > 0) {
        if (value < 0xFF) {
            FxCommon_DrawFullScreenFade((u8)result);
            result = value + 0x10;
        } else {
            FxCommon_DrawFullScreenFade(0xFF);
        }
    }

    if (result < 0) {
        if (result >= -0xFE) {
            FxCommon_DrawFullScreenFade((u8)(result - 1));
            result -= 0x10;
        } else {
            FxCommon_DrawFullScreenFade(0);
            result = 0;
        }
    }

    return result;
}

void FxCommon_DrawFullScreenFade(int intensity)
{
    FxCommonPacketCursor *packet;
    FxCommonBuffer *quadBuffer;
    FxCommonBuffer *modeBuffer;
    FxCommonAddress packetAddress;
    u32 addressMask = 0xFFFFFF;
    u32 commandMask;
    u32 tag;

    packet = g_FxCommonActiveBuffer->data;
    SetPolyG4(&packet->quad);
    SetSemiTrans(&packet->quad, 1);

    packet->quad.color0[0] = intensity;
    packet->quad.color0[1] = intensity;
    packet->quad.color0[2] = intensity;
    packet->quad.color1[0] = intensity;
    packet->quad.color1[1] = intensity;
    packet->quad.color1[2] = intensity;
    packet->quad.color2[0] = intensity;
    packet->quad.color2[1] = intensity;
    packet->quad.color2[2] = intensity;
    packet->quad.color3[0] = intensity;
    packet->quad.color3[1] = intensity;
    packet->quad.color3[2] = intensity;
    packet->quad.x0 = 0;
    packet->quad.y0 = 0;
    packet->quad.x1 = 0x140;
    packet->quad.y1 = 0;
    packet->quad.x2 = 0;
    packet->quad.y2 = 0xF0;
    packet->quad.x3 = 0x140;
    packet->quad.y3 = 0xF0;

    commandMask = 0xFF000000;
    packetAddress.pointer = packet;
    quadBuffer = g_FxCommonActiveBuffer;
    tag = packet->quad.tag;
    tag &= commandMask;
    tag |= *(u32 *)quadBuffer->allocation & addressMask;
    packet->quad.tag = tag;
    *(u32 *)quadBuffer->allocation =
        (*(u32 *)quadBuffer->allocation & commandMask) |
        (packetAddress.word & addressMask);
    packet++;
    quadBuffer->data = packet;

    SetDrawTPage(&packet->mode.packet, 0, 0, 0x40);
    modeBuffer = g_FxCommonActiveBuffer;
    packet->mode.packet.tag.packed = (packet->mode.packet.tag.packed & commandMask) |
                              (*(u32 *)modeBuffer->allocation & addressMask);
    packetAddress.pointer = packet;
    addressMask = packetAddress.word & addressMask;
    *(u32 *)modeBuffer->allocation =
        (*(u32 *)modeBuffer->allocation & commandMask) | addressMask;
    modeBuffer->data = &packet->mode.packet + 1;
}

/* Advance effect selection, resource transitions and the two motion blends. */
void FxCommon_StepEffects(void)
{
    FxCommonMotionRecord mode1Sample;
    FxCommonMotionRecord mode1NextSample;
    FxCommonMotionRecord mode2Sample;
    FxCommonMotionRecord mode2NextSample;
    GteShortVector shortA;
    GteShortVector shortB;
    GteShortVector sampleExtra;
    FxCommonMotionRecord motionSample;
    GteShortVector motionExtra;
    s32 copyX;
    s32 copyY;
    s32 copyZ;
    s32 currentX;
    s32 currentY;
    s32 currentZ;
    s16 resourceResult;
    s32 rowIndex;
    /* Raw halfword is loaded before the blend duration, then sign-extended. */
    u16 phase0;
    u16 phase1;
    s16 remaining0;
    s16 remaining1;
    s16 motionBase0;
    s16 motionBase1;
    s16 countdown;
    s16 transitionMode;
    s32 accumX0;
    s32 accumX1;
    s32 accumZ0;
    s32 accumZ1;
    s32 nextFrame;
    s32 elapsed;
    s32 resourceTicks;
    s32 accumY1;
    s32 accumY0;
    s32 motionSlot0;
    s32 motionSlot1;
    u32 motionIndex0;
    u32 motionIndex1;
    s16 nextIndex0;
    s16 nextIndex1;

    resourceResult = 0;
    if (g_FxCommonExitCountdown != 0) {
        countdown = g_FxCommonExitCountdown - 1;
        g_FxCommonExitCountdown = countdown;
        if (countdown == 1) {
            g_FxCommonExitRequested = 1;
        }
    }
    if (g_FxCommonPlayAdvance == 1) {
        FxCommon_StartCameraMove(g_FxCommonRoomVariant, 0, 0, g_FxCommonPlayFrame << 8);
        rowIndex = g_FxCommonRoomVariant;
        if (g_FxCommonPlayFrame >= g_FxCommonEffectSetup[rowIndex].value30) {
            g_FxCommonDrawTintedCopies = 0;
        }
        if (g_FxCommonPlayFrame < (g_FxCommonEffectSetup[rowIndex].value0c - 1)) {
            g_FxCommonPlayFrame += 1;
        } else {
            g_FxCommonPlayAdvance = 0;
        }
    }
    if ((g_FxCommonExitCountdown != 0) && (g_FxCommonPlayAdvance == 0) && (g_FxCommonViewMode != 0xA) && (g_FxCommonReplayCamera == 0) && (g_FxCommonAutoPlay == -1)) {
        nextFrame = g_FxCommonPlayFrame + 1;
        g_FxCommonPlayFrame = nextFrame;
        FxCommon_StartCameraMove(g_FxCommonRoomVariant, 0, 0, nextFrame << 8);
    }
    if ((g_FxCommonPlaying == 1) && (elapsed = g_FxCommonPlayTicks + 1, g_FxCommonPlayTicks = elapsed, ((elapsed < 0xC9) == 0)) && (g_FxCommonCameraPathRemaining == 0)) {
        if (g_FxCommonReplayCamera == 0) {
            g_FxCommonCameraPathPosition = 0;
        }
        g_FxCommonReplayCamera = 1;
    } else {
        g_FxCommonReplayCamera = 0;
    }
    if (g_FxCommonViewMode != 0xA) {
        if ((g_FxCommonExitCountdown == 0) && (g_FieldPadBits != 0)) {
            if ((g_FieldPadBits & 0x0F000006) == 0x0F000006) {
                VSync(0);
                SetDispMask(0);
                g_FxCommonResetRequested = 1;
                g_FxCommonExitRequested = 1;
                Boot_Shutdown();
            }
            if ((g_FieldPadPressed & 2) && (g_FxCommonAutoPlay == -1)) {
                if (g_FxCommonViewMode != 0xA) {
                    g_FxCommonViewMode = (s16)(g_FxCommonViewMode + 1) % 3;
                }
                if (g_FxCommonViewMode == 0) {
                    g_FxCommonMarkerFade = 1;
                    g_FxCommonPlayAdvance = 0;
                    FxCommon_StartCameraMove(g_FxCommonRoomVariant, 4, 5, 0);
                    if ((g_FxCommonExitCountdown == 0) && (g_AkaoBgmHandle[0] != 0)) {
                        Akao_SendTableCommand(g_AkaoBgmHandle[0], 0x44D, 0, 0x80, 0x7F);
                    }
                }
                transitionMode = g_FxCommonViewMode;
                if (transitionMode == 1) {
                    if (g_FieldPadBits & 0x20) {
                        if ((g_FxCommonExitCountdown == 0) && (g_AkaoBgmHandle[0] != 0)) {
                            Akao_SendTableCommand(g_AkaoBgmHandle[0], 0x44C, 0, 0x80, 0x7F);
                        }
                        g_FxCommonMarkerFade = -1;
                        g_FxCommonDrawTintedCopies = 1;
                        g_FxCommonPlayTicks = 0;
                        g_FxCommonPlaying = 0;
                        g_FxCommonPlayAdvance = 0;
                        g_FxCommonInputLock = 0;
                        FxCommon_SamplePath(g_FxCommonTailPathPosition + 0x20, 0x4A, Draw_GetTimTableEntry((&g_FxCommonTimTable), 2), &mode1Sample.vector, &sampleExtra);
                        FxCommon_SamplePath(g_FxCommonTailPathPosition + 0x20, 0x4B, Draw_GetTimTableEntry((&g_FxCommonTimTable), 2), &mode1NextSample.vector, &sampleExtra);
                        copyX = mode1Sample.vector.x;
                        copyY = mode1Sample.vector.y;
                        copyZ = mode1Sample.vector.z;
                        currentX = g_FxCommonCameraTarget.x;
                        currentY = g_FxCommonCameraTarget.y;
                        currentZ = g_FxCommonCameraTarget.z;
                        shortA.x = (s16)copyX;
                        shortA.y = copyY - 0x64;
                        shortA.z = (s16)copyZ;
                        shortB.x = (s16)currentX;
                        shortB.y = (s16)currentY;
                        shortB.z = (s16)currentZ;
                        FxCommon_SetMotionEndpoints((FxCommonShortVec3 *)&shortA, (FxCommonShortVec3 *)&shortB, 4, 5);
                        g_FxCommonTailViewEntered = 1;
                    } else {
                        g_FxCommonViewMode = 2;
                        g_FxCommonTailViewEntered = 0;
                    }
                }
                if (g_FxCommonViewMode == 2) {
                    if ((g_FxCommonExitCountdown == 0) && (g_AkaoBgmHandle[0] != 0)) {
                        Akao_SendTableCommand(g_AkaoBgmHandle[0], 0x44C, 0, 0x80, 0x7F);
                    }
                    if (g_FxCommonTailViewEntered == 0) {
                        g_FxCommonMarkerFade = -1;
                    }
                    if (g_FxCommonTailViewEntered == 1) {
                        g_FxCommonMarkerFade = 0;
                    }
                    g_FxCommonDrawTintedCopies = 1;
                    g_FxCommonPlayTicks = 0;
                    g_FxCommonPlaying = 0;
                    g_FxCommonPlayAdvance = 0;
                    g_FxCommonInputLock = 0;
                    FxCommon_SamplePath(g_FxCommonFlyerPathPosition + 0x100, 0x4C, Draw_GetTimTableEntry((&g_FxCommonTimTable), 2), &mode2Sample.vector, &sampleExtra);
                    FxCommon_SamplePath(g_FxCommonFlyerPathPosition + 0x100, 0x4D, Draw_GetTimTableEntry((&g_FxCommonTimTable), 2), &mode2NextSample.vector, &sampleExtra);
                    shortA.x = (s16) mode2NextSample.vector.x;
                    shortA.y = (s16) mode2NextSample.vector.y;
                    shortA.z = (s16) mode2NextSample.vector.z;
                    shortB.x = (s16) mode2Sample.vector.x;
                    shortB.y = (s16) mode2Sample.vector.y;
                    shortB.z = (s16) mode2Sample.vector.z;
                    FxCommon_SetMotionEndpoints((FxCommonShortVec3 *)&shortA, (FxCommonShortVec3 *)&shortB, 4, 5);
                }
            }
            if ((g_FieldPadPressed & 0x50) && (g_FxCommonViewMode != 0)) {
                g_FxCommonViewMode = 0;
                g_FxCommonMarkerFade = 1;
                g_FxCommonPlayAdvance = 0;
                FxCommon_StartCameraMove(g_FxCommonRoomVariant, 4, 5, 0);
                if ((g_FxCommonExitCountdown == 0) && (g_AkaoBgmHandle[0] != 0)) {
                    Akao_SendTableCommand(g_AkaoBgmHandle[0], 0x44D, 0, 0x80, 0x7F);
                }
            }
            if ((g_FxCommonCameraPathRemaining < 2U) && (g_FxCommonViewMode == 0) && (g_FxCommonInputLock == 0)) {
                if ((g_FieldPadPressed & 0x20000000) && (g_FxCommonExitCountdown == 0)) {
                    if (g_FxCommonCameraPathRemaining == 1) {
                        g_FxCommonScreenFade = 1;
                        g_FxCommonExitCountdown = 0x10;
                        g_FxCommonExitVariant = g_FxCommonRoomVariant;
                    }
                    if (g_FxCommonPlaying == 0) {
                        if (g_AkaoBgmHandle[0] != 0) {
                            Akao_SendTableCommand(g_AkaoBgmHandle[0], 0x44C, 0, 0x80, 0x7F);
                        }
                        g_FxCommonMarkerFade = -1;
                        g_FxCommonPlayAdvance = 1;
                        g_FxCommonPlaying = 1;
                        g_FxCommonPlayFrame = 0;
                        g_FxCommonPlayTicks = 1;
                    } else {
                        if (g_AkaoBgmHandle[0] != 0) {
                            Akao_SendTableCommand(g_AkaoBgmHandle[0], 0x44C, 0, 0x80, 0x7F);
                        }
                        g_FxCommonScreenFade = 1;
                        g_FxCommonExitCountdown = 0x10;
                        g_FxCommonExitVariant = g_FxCommonRoomVariant;
                    }
                    if ((g_FxCommonConfirmExit == 1) && (g_FxCommonRoomVariant == g_FxCommonConfirmExitVariant)) {
                        g_FxCommonMarkerFade = -1;
                        g_FxCommonScreenFade = 1;
                        g_FxCommonPlayAdvance = 0;
                        g_FxCommonPlaying = 0;
                        g_FxCommonPlayFrame = 0;
                        g_FxCommonPlayTicks = 0;
                        g_FxCommonExitCountdown = 0x10;
                        g_FxCommonExitVariant = g_FxCommonRoomVariant;
                    }
                }
                if ((g_FieldPadPressed & 0x40000000) && (g_FxCommonAutoPlay == -1) && (g_FxCommonExitCountdown == 0)) {
                    g_FxCommonDrawTintedCopies = 1;
                    if (g_FxCommonPlaying == 1) {
                        if (g_AkaoBgmHandle[0] != 0) {
                            Akao_SendTableCommand(g_AkaoBgmHandle[0], 0x44D, 0, 0x80, 0x7F);
                        }
                        FxCommon_StartCameraMove(g_FxCommonRoomVariant, 4, 5, 0);
                        g_FxCommonMarkerFade = 1;
                        g_FxCommonPlaying = 0;
                        g_FxCommonPlayAdvance = 0;
                    }
                }
                if ((g_FieldPadPressed & 0x40) && (g_FxCommonAutoPlay == -1)) {
                    if (g_FxCommonPlaying == 1) {
                        g_FxCommonMarkerFade = 1;
                    }
                    g_FxCommonPlayAdvance = 0;
                    g_FxCommonPlaying = 0;
                    g_FxCommonPlayFrame = 0;
                    g_FxCommonDrawTintedCopies = 1;
                    if (g_FxCommonEffectSetup[g_FxCommonRoomVariant].value22 != 0xFF) {
                        do {
                            nextIndex0 = g_FxCommonEffectSetup[g_FxCommonRoomVariant].value22;
                            g_FxCommonRoomVariant = nextIndex0;
                        } while (g_FxCommonEffectSetup[nextIndex0].enabled == 0);
                        g_FxCommonPageChanged = 1;
                        g_FxCommonPlayFrame = g_FxCommonEffectSetup[nextIndex0].value0c;
                        FxCommon_StartCameraMove(nextIndex0, 4, 5, 0);
                        motionIndex0 = 0;
                        motionSlot0 = 0;
                        phase0 = g_FxCommonRoomVariant;
                        g_FxCommonMotionBlendFrames = 0x20;
                        motionBase0 = (s16)phase0;
                        do {
                            FxCommon_SamplePath(motionIndex0 << 8, motionBase0 + 0x40, Draw_GetTimTableEntry((&g_FxCommonTimTable), 2), &motionSample.vector, &motionExtra);
                            motionIndex0 += 1;
                            g_FxCommonMotionDeltas[motionSlot0].vector.x = ((motionSample.vector.x << 0x10) - g_FxCommonMotionPositions[motionSlot0].vector.x) >> 5;
                            g_FxCommonMotionDeltas[motionSlot0].vector.y = ((motionSample.vector.y << 0x10) - g_FxCommonMotionPositions[motionSlot0].vector.y) >> 5;
                            g_FxCommonMotionDeltas[motionSlot0].vector.z = ((motionSample.vector.z << 0x10) - g_FxCommonMotionPositions[motionSlot0].vector.z) >> 5;
                            motionSlot0 += 1;
                        } while (motionIndex0 < 0xAU);
                    }
                }
                if ((g_FieldPadPressed & 0x10) && (g_FxCommonAutoPlay == -1)) {
                    if (g_FxCommonPlaying == 1) {
                        g_FxCommonMarkerFade = 1;
                    }
                    g_FxCommonPlayAdvance = 0;
                    g_FxCommonPlaying = 0;
                    g_FxCommonPlayFrame = 0;
                    g_FxCommonDrawTintedCopies = 1;
                    if (g_FxCommonEffectSetup[g_FxCommonRoomVariant].value23 != 0xFF) {
                        do {
                            nextIndex1 = g_FxCommonEffectSetup[g_FxCommonRoomVariant].value23;
                            g_FxCommonRoomVariant = nextIndex1;
                        } while (g_FxCommonEffectSetup[nextIndex1].enabled == 0);
                        g_FxCommonPageChanged = 1;
                        g_FxCommonPlayFrame = g_FxCommonEffectSetup[nextIndex1].value0c;
                        FxCommon_StartCameraMove(nextIndex1, 4, 5, 0);
                        motionIndex1 = 0;
                        motionSlot1 = 0;
                        phase1 = g_FxCommonRoomVariant;
                        g_FxCommonMotionBlendFrames = 0x20;
                        motionBase1 = (s16)phase1;
                        do {
                            FxCommon_SamplePath(motionIndex1 << 8, motionBase1 + 0x40, Draw_GetTimTableEntry((&g_FxCommonTimTable), 2), &motionSample.vector, &motionExtra);
                            motionIndex1 += 1;
                            g_FxCommonMotionDeltas[motionSlot1].vector.x = ((motionSample.vector.x << 0x10) - g_FxCommonMotionPositions[motionSlot1].vector.x) >> 5;
                            g_FxCommonMotionDeltas[motionSlot1].vector.y = ((motionSample.vector.y << 0x10) - g_FxCommonMotionPositions[motionSlot1].vector.y) >> 5;
                            g_FxCommonMotionDeltas[motionSlot1].vector.z = ((motionSample.vector.z << 0x10) - g_FxCommonMotionPositions[motionSlot1].vector.z) >> 5;
                            motionSlot1 += 1;
                        } while (motionIndex1 < 0xAU);
                    }
                }
            }
        }
    }
    if ((g_FxCommonViewMode == 1) && (g_FxCommonMotionRemaining0 == 0) && (g_FxCommonMotionRemaining1 == 0)) {
        ((FxCommonVec3 *)&g_FxCommonCameraPosition)->x = g_FxCommonPathTailNodes[1]->matrix.t[0];
        g_FxCommonCameraPosition.y = g_FxCommonPathTailNodes[1]->matrix.t[1] - 0x12C;
        g_FxCommonCameraPosition.z = g_FxCommonPathTailNodes[1]->matrix.t[2];
        ((FxCommonVec3 *)&g_FxCommonCameraTarget)->x = g_FxCommonPathNodes[0]->matrix.t[0];
        g_FxCommonCameraTarget.y = g_FxCommonPathNodes[0]->matrix.t[1];
        g_FxCommonCameraTarget.z = g_FxCommonPathNodes[0]->matrix.t[2];
    }
    if ((g_FxCommonViewMode == 2) && (g_FxCommonMotionRemaining0 == 0) && (g_FxCommonMotionRemaining1 == 0)) {
        ((FxCommonVec3 *)&g_FxCommonCameraPosition)->x = g_FxCommonPathNodes[2]->matrix.t[0];
        g_FxCommonCameraPosition.y = g_FxCommonPathNodes[2]->matrix.t[1];
        g_FxCommonCameraPosition.z = g_FxCommonPathNodes[2]->matrix.t[2];
        ((FxCommonVec3 *)&g_FxCommonCameraTarget)->x = g_FxCommonPathNodes[0]->matrix.t[0];
        g_FxCommonCameraTarget.y = g_FxCommonPathNodes[0]->matrix.t[1];
        g_FxCommonCameraTarget.z = g_FxCommonPathNodes[0]->matrix.t[2];
    }
    if (g_FxCommonReplayCamera == 1) {
        FxCommon_SamplePath(g_FxCommonCameraPathPosition, g_FxCommonEffectSetup[g_FxCommonRoomVariant].motionIndices[3], Draw_GetTimTableEntry((&g_FxCommonTimTable), 2), &g_FxCommonCameraPosition, &sampleExtra);
        FxCommon_SamplePath(g_FxCommonCameraPathPosition, g_FxCommonEffectSetup[g_FxCommonRoomVariant].motionIndices[2], Draw_GetTimTableEntry((&g_FxCommonTimTable), 2), &g_FxCommonCameraTarget, &sampleExtra);
        g_FxCommonCameraPathPosition += 0x80;
    }
    if (g_FxCommonViewMode == 0xA) {
        g_FxCommonDemoFrame += 1;
        FxCommon_SamplePath(g_FxCommonCameraPathPosition, 0x4E, Draw_GetTimTableEntry((&g_FxCommonTimTable), 2), &g_FxCommonCameraPosition, &sampleExtra);
        FxCommon_SamplePath(g_FxCommonCameraPathPosition, 0x4F, Draw_GetTimTableEntry((&g_FxCommonTimTable), 2), &g_FxCommonCameraTarget, &sampleExtra);
        g_FxCommonCameraPathPosition += 0x10;
        if ((s16) g_FxCommonDemoFrame == 0x4B) {
            g_FxCommonDemoResource = FxCommon_SwitchResource(0xB, 0U);
            g_FxCommonDemoResourceTicks = 0;
        }
        if ((s16)g_FxCommonDemoFrame == 0x11D) {
            FxCommon_SwitchResource(0xE, 1U);
        }
        if ((s16)g_FxCommonDemoFrame == 0x13B) {
            g_FxCommonDemoResource = FxCommon_SwitchResource(0xC, 0U);
            g_FxCommonDemoResourceTicks = 0;
        }
        if ((s16)g_FxCommonDemoFrame == 0x20D) {
            FxCommon_SwitchResource(0xE, 1U);
        }
        if ((s16)g_FxCommonDemoFrame == 0x22B) {
            g_FxCommonDemoResource = FxCommon_SwitchResource(0xD, 0U);
            g_FxCommonDemoResourceTicks = 0;
        }
        if ((s16)g_FxCommonDemoFrame == 0x2FD) {
            FxCommon_SwitchResource(0xE, 1U);
        }
        if ((s16)g_FxCommonDemoFrame == 0x31B) {
            g_FxCommonDemoResource = FxCommon_SwitchResource(0xE, 0U);
            g_FxCommonDemoResourceTicks = 0;
        }
        if ((s16)g_FxCommonDemoFrame == 0x3ED) {
            FxCommon_SwitchResource(0xE, 1U);
        }
        if ((s16)g_FxCommonDemoFrame == 0x40B) {
            g_FxCommonDemoResource = FxCommon_SwitchResource(0xE, 1U);
        }
        if (g_FxCommonDemoResourceTicks == 0) {
            g_FxCommonDemoFadePhase = 0;
            g_FxCommonDemoShade = 0;
        }
        if (g_FxCommonDemoResourceTicks == 0x18) {
            g_FxCommonDemoFadePhase = 1;
        }
        if (g_FxCommonDemoResourceTicks == 0xBA) {
            g_FxCommonDemoFadePhase = 2;
        }
        g_FxCommonDemoResourceTicks += 1;
        if ((g_FxCommonDemoFadePhase == 0) && (g_FxCommonDemoShade < 0x78U)) {
            g_FxCommonDemoShade += 5;
        }
        if ((g_FxCommonDemoFadePhase == 2) && (g_FxCommonDemoShade >= 0xBU)) {
            g_FxCommonDemoShade -= 5;
        }
        Menu_SetHighlightColor(g_FxCommonDemoResource, g_FxCommonDemoShade, g_FxCommonDemoShade, g_FxCommonDemoShade);
        resourceTicks = g_FxCommonDemoTicks + 1;
        g_FxCommonDemoTicks = resourceTicks;
        if (resourceTicks == 0x4B1) {
            g_FxCommonScreenFade = 1;
            g_FxCommonExitCountdown = 0x10;
            g_FxCommonExitVariant = g_FxCommonRoomVariant;
            Akao_Cmd_A9(0x12C, 0);
            g_FxCommonDemoExiting = 1;
        }
        if (g_FxCommonDemoTicks == 0x41A) {
            Akao_Cmd_C1_WithSlot(g_SceneAudioSlot0Handle, 0xDC, 0);
        }
    }
    FxCommon_StepCameraPath();
    if (g_FxCommonPageChanged != 0) {
        g_FxCommonPageChanged = 0;
        if (g_FxCommonPageOpen == 1) {
            Draw_CloseTextboxPage(g_FxCommonPageId);
        }
        Menu_SetHighlightColor(g_FxCommonEffectSetup[g_FxCommonRoomVariant].value25, 0x80, 0x80, 0x80);
        if (g_FxCommonViewMode != 0xA) {
            Render_SetupColorTable(g_FxCommonEffectSetup[g_FxCommonRoomVariant].value25, 3, &resourceResult);
        }
        g_FxCommonPageId = (s16) g_FxCommonEffectSetup[g_FxCommonRoomVariant].value25;
        g_FxCommonPageOpen = 1;
    }
    if (g_FxCommonMotionRemaining0 != 0) {
        remaining0 = g_FxCommonMotionRemaining0 - 1;
        g_FxCommonMotionRemaining0 = remaining0;
        accumX0 = g_FxCommonMotionAccum0.x + g_FxCommonMotionStep0X;
        g_FxCommonMotionAccum0.x = accumX0;
        accumY0 = g_FxCommonMotionAccum0.y + g_FxCommonMotionStep0Y;
        accumZ0 = g_FxCommonMotionAccum0.z + g_FxCommonMotionStep0Z;
        g_FxCommonMotionAccum0.y = accumY0;
        g_FxCommonMotionAccum0.z = accumZ0;
        g_FxCommonCameraTarget.x = g_FxCommonMotionOrigin0.x + (accumX0 >> g_FxCommonMotionShift0);
        g_FxCommonCameraTarget.y = g_FxCommonMotionOrigin0.y + (accumY0 >> g_FxCommonMotionShift0);
        g_FxCommonCameraTarget.z = g_FxCommonMotionOrigin0.z + (accumZ0 >> g_FxCommonMotionShift0);
        if ((remaining0 << 0x10) == 0) {
            g_FxCommonCameraTarget.x = g_FxCommonMotionTarget0.x;
            g_FxCommonCameraTarget.y = g_FxCommonMotionTarget0.y;
            g_FxCommonCameraTarget.z = g_FxCommonMotionTarget0.z;
        }
    }
    if (g_FxCommonMotionRemaining1 != 0) {
        remaining1 = g_FxCommonMotionRemaining1 - 1;
        g_FxCommonMotionRemaining1 = remaining1;
        accumX1 = g_FxCommonMotionAccum1.x + g_FxCommonMotionStep1X;
        g_FxCommonMotionAccum1.x = accumX1;
        accumY1 = g_FxCommonMotionAccum1.y + g_FxCommonMotionStep1Y;
        accumZ1 = g_FxCommonMotionAccum1.z + g_FxCommonMotionStep1Z;
        g_FxCommonMotionAccum1.y = accumY1;
        g_FxCommonMotionAccum1.z = accumZ1;
        g_FxCommonCameraPosition.x = g_FxCommonMotionOrigin1.x + (accumX1 >> g_FxCommonMotionShift1);
        g_FxCommonCameraPosition.y = g_FxCommonMotionOrigin1.y + (accumY1 >> g_FxCommonMotionShift1);
        g_FxCommonCameraPosition.z = g_FxCommonMotionOrigin1.z + (accumZ1 >> g_FxCommonMotionShift1);
        if ((remaining1 << 0x10) == 0) {
            g_FxCommonCameraPosition.x = g_FxCommonMotionTarget1.x;
            g_FxCommonCameraPosition.y = g_FxCommonMotionTarget1.y;
            g_FxCommonCameraPosition.z = g_FxCommonMotionTarget1.z;
        }
    }
}

#undef DRAW
#undef DRAW_PAIR
#undef FX_MODE_PACKET

s16 FxCommon_SwitchResource(s16 resourceId, u8 action)
{
    s16 result;
    s16 id;

    result = 0;
    id = resourceId;
    if (action == 0) {
        if (id != g_FxCommonResourceState.active) {
            Draw_CloseTextboxPage(g_FxCommonResourceState.current.half);
            Menu_SetHighlightColor(id, 0x80, 0x80, 0x80);
            Render_SetupColorTable(id, 3, &result);
            g_FxCommonResourceState.current.word = id;
        }
    }

    if (action == 1) {
        Draw_CloseTextboxPage(g_FxCommonResourceState.current.half);
    }

    return resourceId;
}

/* Pins and empty scheduling barriers preserve the retail argument setup,
 * interleaved vector reads, and snapshot stores; tracked in crutch debt. */
void FxCommon_StartCameraMove(s32 index, s32 first, s32 second, u32 source) {
    FxCommonMotionRecord point;
    GteShortVector extra;
    s32 secondMask;
    s16 setupIndex;
    s32 firstMask;
    register s32 firstValue asm("$18");
    s32 secondValue;
    TimRelativeTable *table = &g_FxCommonTimTable;
    GteShortVector *extraPtr;
    volatile FxCommonShortVec3 *origin0 = &g_FxCommonMotionOrigin0;
    volatile FxCommonShortVec3 *target0 = &g_FxCommonMotionTarget0;
    volatile FxCommonShortVec3 *origin1 = &g_FxCommonMotionOrigin1;
    volatile FxCommonShortVec3 *target1 = &g_FxCommonMotionTarget1;
    register u32 sourceValue asm("$22");
    register TimRelativeTable *lookupTable asm("$4");
    register s32 lookupCount asm("$5");
    void *base;
    register s32 bx asm("$9");
    s32 by;
    s32 bz;
    s32 ax;
    s32 ay;
    s32 az;
    s32 px;
    s32 py;
    s32 pz;
    s32 qx;
    s32 qy;
    s32 qz;

    lookupTable = table;
    firstValue = first;
    lookupCount = 2;
    sourceValue = source;
    /* Capture the second lane index after preparing the lookup arguments. */
    secondValue = second;
    asm("" : "=r"(secondValue) : "0"(secondValue), "r"(lookupTable), "r"(lookupCount), "r"(sourceValue));
    base = Draw_GetTimTableEntry(lookupTable, lookupCount);
    setupIndex = (s16)index;
    extraPtr = &extra;
    FxCommon_SamplePath(sourceValue, g_FxCommonEffectSetup[setupIndex].motionIndices[0], base, &point.vector, extraPtr);

    lookupTable = table;
    firstValue &= 0xff;
    px = point.vector.x;
    bx = g_FxCommonCameraTarget.x;
    asm volatile("" : : "r"(bx));
    py = point.vector.y;
    by = g_FxCommonCameraTarget.y;
    pz = point.vector.z;
    bz = g_FxCommonCameraTarget.z;
    g_FxCommonMotionShift0 = firstValue;
    firstMask = 1 << firstValue;
    g_FxCommonMotionAccum0X = 0;
    g_FxCommonMotionAccum0Y = 0;
    g_FxCommonMotionAccum0Z = 0;
    g_FxCommonMotionRemaining0 = firstMask;
    g_FxCommonMotionStep0X = px - bx;
    g_FxCommonMotionStep0Y = py - by;
    g_FxCommonMotionStep0Z = pz - bz;
    g_FxCommonMotionOrigin0X = bx;
    origin0->y = by;
    origin0->z = bz;
    g_FxCommonMotionTarget0X = px;
    target0->y = py;
    target0->z = pz;

    base = Draw_GetTimTableEntry(lookupTable, 2);
    FxCommon_SamplePath(sourceValue, g_FxCommonEffectSetup[setupIndex].motionIndices[1], base, &point.vector, extraPtr);

    asm volatile("" : : "r"(secondValue));
    secondValue &= 0xff;
    qx = point.vector.x;
    ax = g_FxCommonCameraPosition.x;
    qy = point.vector.y;
    asm volatile("" : : "r"(qy));
    ay = g_FxCommonCameraPosition.y;
    asm volatile("" : : "r"(ay));
    qz = point.vector.z;
    asm volatile("" : : "r"(qz));
    az = g_FxCommonCameraPosition.z;
    asm volatile("" : : "r"(az));
    secondMask = 1 << secondValue;
    g_FxCommonMotionAccum1X = 0;
    g_FxCommonMotionAccum1Y = 0;
    g_FxCommonMotionAccum1Z = 0;
    g_FxCommonMotionShift1 = secondValue;
    g_FxCommonMotionRemaining1 = secondMask;
    g_FxCommonMotionStep1X = qx - ax;
    g_FxCommonMotionStep1Y = qy - ay;
    g_FxCommonMotionStep1Z = qz - az;
    g_FxCommonMotionOrigin1X = ax;
    origin1->y = ay;
    origin1->z = az;
    g_FxCommonMotionTarget1X = qx;
    target1->y = qy;
    target1->z = qz;
}
