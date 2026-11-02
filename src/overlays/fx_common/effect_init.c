/* CC1_FLAGS: -fno-expensive-optimizations */

#include "fx_common_motion.h"
#include "fx_common_setup.h"

#define SET_RGB(color, r, g, b) \
    (color)[0] = (r);           \
    (color)[1] = (g);           \
    (color)[2] = (b)

/* Build both frame buffers' full-screen gradient quads (sky, shade, the
 * semi-transparent bottom and top fades) and their three draw modes. */
void FxCommon_InitFadeQuads(void)
{
    u32 i;

    for (i = 0; i < 2; i++) {
        SetPolyG4(&g_FxCommonSkyQuads[i]);
        SET_RGB(g_FxCommonSkyQuads[i].color0, 20, 68, 100);
        SET_RGB(g_FxCommonSkyQuads[i].color1, 20, 68, 100);
        SET_RGB(g_FxCommonSkyQuads[i].color2, 5, 17, 25);
        SET_RGB(g_FxCommonSkyQuads[i].color3, 5, 17, 25);
        g_FxCommonSkyQuads[i].x0 = 0;
        g_FxCommonSkyQuads[i].y0 = 0;
        g_FxCommonSkyQuads[i].x1 = 320;
        g_FxCommonSkyQuads[i].y1 = 0;
        g_FxCommonSkyQuads[i].x2 = 0;
        g_FxCommonSkyQuads[i].y2 = 240;
        g_FxCommonSkyQuads[i].x3 = 320;
        g_FxCommonSkyQuads[i].y3 = 240;

        SetPolyG4(&g_FxCommonShadeQuads[i]);
        SET_RGB(g_FxCommonShadeQuads[i].color0, 10, 24, 40);
        SET_RGB(g_FxCommonShadeQuads[i].color1, 10, 24, 40);
        SET_RGB(g_FxCommonShadeQuads[i].color2, 5, 7, 15);
        SET_RGB(g_FxCommonShadeQuads[i].color3, 5, 7, 15);
        g_FxCommonShadeQuads[i].x0 = 0;
        g_FxCommonShadeQuads[i].y0 = 0;
        g_FxCommonShadeQuads[i].x1 = 320;
        g_FxCommonShadeQuads[i].y1 = 0;
        g_FxCommonShadeQuads[i].x2 = 0;
        g_FxCommonShadeQuads[i].y2 = 240;
        g_FxCommonShadeQuads[i].x3 = 320;
        g_FxCommonShadeQuads[i].y3 = 240;

        SetPolyG4(&g_FxCommonFloorFadeQuads[i]);
        SetSemiTrans(&g_FxCommonFloorFadeQuads[i], 1);
        SET_RGB(g_FxCommonFloorFadeQuads[i].color0, 0, 0, 0);
        SET_RGB(g_FxCommonFloorFadeQuads[i].color1, 0, 0, 0);
        SET_RGB(g_FxCommonFloorFadeQuads[i].color2, 255, 255, 255);
        SET_RGB(g_FxCommonFloorFadeQuads[i].color3, 255, 255, 255);
        g_FxCommonFloorFadeQuads[i].x0 = 0;
        g_FxCommonFloorFadeQuads[i].y0 = 200;
        g_FxCommonFloorFadeQuads[i].x1 = 320;
        g_FxCommonFloorFadeQuads[i].y1 = 200;
        g_FxCommonFloorFadeQuads[i].x2 = 0;
        g_FxCommonFloorFadeQuads[i].y2 = 240;
        g_FxCommonFloorFadeQuads[i].x3 = 320;
        g_FxCommonFloorFadeQuads[i].y3 = 240;

        SetPolyG4(&g_FxCommonTopFadeQuads[i]);
        SetSemiTrans(&g_FxCommonTopFadeQuads[i], 1);
        SET_RGB(g_FxCommonTopFadeQuads[i].color0, 255, 255, 255);
        SET_RGB(g_FxCommonTopFadeQuads[i].color1, 255, 255, 255);
        SET_RGB(g_FxCommonTopFadeQuads[i].color2, 0, 0, 0);
        SET_RGB(g_FxCommonTopFadeQuads[i].color3, 0, 0, 0);
        g_FxCommonTopFadeQuads[i].x0 = 0;
        g_FxCommonTopFadeQuads[i].y0 = 0;
        g_FxCommonTopFadeQuads[i].x1 = 320;
        g_FxCommonTopFadeQuads[i].y1 = 0;
        g_FxCommonTopFadeQuads[i].x2 = 0;
        g_FxCommonTopFadeQuads[i].y2 = 40;
        g_FxCommonTopFadeQuads[i].x3 = 320;
        g_FxCommonTopFadeQuads[i].y3 = 40;

        SetDrawTPage(&g_FxCommonDrawModes0[i], 0, 0, 0);
        SetDrawTPage(&g_FxCommonDrawModes1[i], 0, 0, 0x20);
        SetDrawTPage(&g_FxCommonDrawModes2[i], 0, 0, 0x40);
    }
}

void FxCommon_InitScene(void)
{
    s32 firstPointOffset;
    s32 pointIndex;
    FxCommonPoint8 *pointList;
    FxCommonMotionRecord motion;
    GteShortVector shortMotion;
    s32 variantBase;
    s32 textureIndexOrHasPoints;
    u8 *resourceTableData;
    s32 typeFixed;
    s32 subtypeFixed;
    s32 groupVariant;
    s32 resourceOffset;
    FxCommonNode **secondInitialSlot;
    s16 *rotationCursor;
    s16 *zCursor;
    s16 *yCursor;
    FxCommonEffectSetupRecord *setupCursor;
    s16 *typeCursor;
    s16 *variantCursor;
    s16 *initialSubtype;
    FxCommonNode **initialSlots;
    s16 *subtypeCursor;
    FxCommonNode **secondSlot;
    FxCommonNode **firstSlot;
    FxCommonNode *secondNode;
    FxCommonNode *firstInitialNode;
    FxCommonNode *secondInitialNode;
    FxCommonNode *firstNode;
    FxCommonNode *initialNode;
    FxCommonNode *pairedInitialNode;
    FxCommonNode *pairedNode;
    s32 initialNodeSubtype;
    s32 initialNodeVariant;
    s32 initialNodeType;
    register s32 variantOrMotionOffset asm("$16");
    s16 motionBase;
    register s32 pointCount asm("$22");
    FxCommonOffsetTable *resourceTable;
    s32 nodeType;
    s32 nodeSubtype;
    s32 nextPointIndex;
    s32 pointOffset;
    s32 nodeIndex;
    register FxCommonNode *yNode asm("$8");
    s32 groupIndex;
    s32 tableIndex;
    s8 motionCount;
    u16 textureY;
    u16 pointZ;
    FxCommonNode *effectRoot;
    u32 motionIndex;
    s16 selectedMotion;
    void *nodeResource;
    void *initialResource;
    register void *resourceData asm("$3");
    FxCommonPoint8 *point;

    FxCommon_SelectRoomVariant();
    FxCommon_InitNodePools();
    FxCommon_InitFadeQuads();
    SetLightMatrix(&g_FxCommonLightMatrix);
    SetColorMatrix(&g_FxCommonColorMatrix);
    SetBackColor(0x40, 0x40, 0x40);
    SetFogNearFar(0x1000, 0x2000, 0x300);
    SetFarColor(0xFF, 0xFF, 0xFF);
    textureIndexOrHasPoints = 0;
    do {
        textureY = g_FieldBgTextureSlots[textureIndexOrHasPoints].y;
        g_FieldBgTextureSlots[textureIndexOrHasPoints].tpage =
            BG_SLOT_TPAGE(g_FieldBgTextureSlots[textureIndexOrHasPoints].x, textureY);
        g_FieldBgTextureSlots[textureIndexOrHasPoints].clut =
            BG_SLOT_CLUT(g_FieldBgTextureSlots[textureIndexOrHasPoints].clut_x,
                         g_FieldBgTextureSlots[textureIndexOrHasPoints].clut_y);
        textureIndexOrHasPoints++;
    } while (textureIndexOrHasPoints < 2);
    if ((g_FxCommonAltFogLayer != 0) || (Render_SetupFogLayer(Draw_GetTimTableEntry(&g_FxCommonTimTable, 0)), (g_FxCommonAltFogLayer != 0))) {
        Render_SetupFogLayer(Draw_GetTimTableEntry(&g_FxCommonTimTable, 1));
    }
    resourceTableData = &g_FxCommonTimTable;
    resourceTable = Draw_GetTimTableEntry(resourceTableData, 2);
    pointIndex = 0;
    initialSlots = g_FxCommonRuntime.pairedNodes;
    secondInitialSlot = initialSlots + 10;
    initialSubtype = &g_FxCommonEffectSetup[0].subtype;
    /* Preserve the common base used by the seven setup-field cursors. */
    asm("" : "=r"(initialSubtype) : "0"(initialSubtype));
    subtypeCursor = initialSubtype;
    rotationCursor = &((FxCommonEffectSetupRecord *)(initialSubtype - 10))->rotation;
    zCursor = &((FxCommonEffectSetupRecord *)(initialSubtype - 10))->position[2];
    yCursor = &((FxCommonEffectSetupRecord *)(initialSubtype - 10))->position[1];
    setupCursor = (FxCommonEffectSetupRecord *)(initialSubtype - 10);
    typeCursor = &((FxCommonEffectSetupRecord *)(initialSubtype - 10))->type;
    variantCursor = &((FxCommonEffectSetupRecord *)(initialSubtype - 10))->variant;
    firstPointOffset = resourceTable->offsets[0];
    *subtypeCursor = 0x15;
    g_FxCommonEffectSetup[0].variant = 0x16;
    g_FxCommonEffectSetup[0].type = 0x17;
    g_FxCommonEffectSetup[0].motionIndices[0] = 0x1B;
    g_FxCommonEffectSetup[0].motionIndices[1] = 0x1A;
    g_FxCommonEffectSetup[0].motionIndices[2] = 0x2F;
    g_FxCommonEffectSetup[0].motionIndices[3] = 0x2E;
    pointList = (FxCommonPoint8 *)&((u8 *)resourceTable)[firstPointOffset]
        + 1;

    g_FxCommonEffectSetup[0].value0c = 0x62;
    g_FxCommonEffectSetup[0].parameter3c = 0x7D0;
    g_FxCommonEffectSetup[0].value30 = 0x2E;
    g_FxCommonEffectSetup[0].value10 = 0x161;
    g_FxCommonEffectSetup[0].parameter38 = 0xBB8;
    g_FxCommonEffectSetup[0].value22 = 1;
    g_FxCommonEffectSetup[0].value23 = 4;
    g_FxCommonEffectSetup[1].subtype = 0x18;
    g_FxCommonEffectSetup[0].value28 = 0x20;
    g_FxCommonEffectSetup[0].value2c = 0x10;
    g_FxCommonEffectSetup[0].rotation = 0;
    g_FxCommonEffectSetup[0].value25 = 9;
    g_FxCommonEffectSetup[1].variant = 0x19;
    g_FxCommonEffectSetup[0].enabled = (g_ScriptVariableBank[SCRIPT_VAR_FLAGS] >> 0x16) & 1;
    g_FxCommonEffectSetup[1].type = 0x1A;
    g_FxCommonEffectSetup[1].motionIndices[0] = 0xB;
    g_FxCommonEffectSetup[1].motionIndices[2] = 0x1F;
    g_FxCommonEffectSetup[1].motionIndices[3] = 0x1E;
    g_FxCommonEffectSetup[1].motionIndices[1] = 0xA;
    g_FxCommonEffectSetup[1].value0c = 0x4F;
    g_FxCommonEffectSetup[1].value10 = 0x161;
    g_FxCommonEffectSetup[1].parameter38 = 0x1388;
    g_FxCommonEffectSetup[1].parameter3c = 0x9C4;
    g_FxCommonEffectSetup[1].value22 = 2;
    g_FxCommonEffectSetup[1].value23 = 0;
    g_FxCommonEffectSetup[1].enabled = (g_ScriptVariableBank[SCRIPT_VAR_FLAGS] >> 0x14) & 1;
    g_FxCommonEffectSetup[1].value28 = 0x10;
    g_FxCommonEffectSetup[2].subtype = 0xF;
    g_FxCommonEffectSetup[1].value2c = 0x20;
    g_FxCommonEffectSetup[1].rotation = 0;
    g_FxCommonEffectSetup[1].value25 = 7;
    g_FxCommonEffectSetup[1].value30 = 0x1C;
    g_FxCommonEffectSetup[2].variant = 0x10;
    g_FxCommonEffectSetup[2].type = 0x11;
    g_FxCommonEffectSetup[2].motionIndices[0] = 0x19;
    g_FxCommonEffectSetup[2].motionIndices[1] = 0x18;
    g_FxCommonEffectSetup[2].motionIndices[2] = 0x2D;
    g_FxCommonEffectSetup[2].value0c = 0x56;
    g_FxCommonEffectSetup[2].value10 = 0x161;
    g_FxCommonEffectSetup[2].parameter38 = 0x1388;
    g_FxCommonEffectSetup[2].parameter3c = 0x9C4;
    g_FxCommonEffectSetup[2].value22 = 3;
    g_FxCommonEffectSetup[2].value23 = 1;
    g_FxCommonEffectSetup[2].value28 = -0x30;
    g_FxCommonEffectSetup[2].motionIndices[3] = 0x2C;
    g_FxCommonEffectSetup[2].value2c = 0x20;
    g_FxCommonEffectSetup[2].value30 = 0x27;
    g_FxCommonEffectSetup[2].enabled = (g_ScriptVariableBank[SCRIPT_VAR_FLAGS] >> 0x12) & 1;
    g_FxCommonEffectSetup[2].rotation = 0;
    g_FxCommonEffectSetup[2].value25 = 5;
    g_FxCommonEffectSetup[3].type = 0x1D;
    g_FxCommonEffectSetup[3].variant = 0x1C;
    g_FxCommonEffectSetup[3].motionIndices[0] = 0x1D;
    g_FxCommonEffectSetup[3].motionIndices[1] = 0x1C;
    g_FxCommonEffectSetup[3].motionIndices[2] = 0x31;
    g_FxCommonEffectSetup[3].subtype = 0x1B;
    g_FxCommonEffectSetup[3].value0c = 0x56;
    g_FxCommonEffectSetup[3].motionIndices[3] = 0x30;
    g_FxCommonEffectSetup[3].value10 = 0x161;
    g_FxCommonEffectSetup[3].parameter3c = 0xDAC;
    g_FxCommonEffectSetup[3].parameter38 = 0x1388;
    g_FxCommonEffectSetup[3].value22 = 6;
    g_FxCommonEffectSetup[3].value23 = 2;
    g_FxCommonEffectSetup[3].value28 = -0x40;
    g_FxCommonEffectSetup[3].value30 = 0x1E;
    g_FxCommonEffectSetup[3].enabled = (g_ScriptVariableBank[SCRIPT_VAR_FLAGS] >> 0x15) & 1;
    g_FxCommonEffectSetup[3].value2c = 0x10;
    g_FxCommonEffectSetup[4].subtype = 9;
    g_FxCommonEffectSetup[3].rotation = 0;
    g_FxCommonEffectSetup[4].variant = 0xA;
    g_FxCommonEffectSetup[3].value25 = 8;
    g_FxCommonEffectSetup[4].motionIndices[0] = 0x13;
    g_FxCommonEffectSetup[4].motionIndices[1] = 0x12;
    g_FxCommonEffectSetup[4].motionIndices[2] = 0x27;
    g_FxCommonEffectSetup[4].motionIndices[3] = 0x26;
    g_FxCommonEffectSetup[4].type = 0xB;
    g_FxCommonEffectSetup[4].value0c = 0x54;
    g_FxCommonEffectSetup[4].value10 = 0x161;
    g_FxCommonEffectSetup[4].parameter38 = 0x1388;
    g_FxCommonEffectSetup[4].parameter3c = 0xBB8;
    g_FxCommonEffectSetup[4].value2c = -0x10;
    g_FxCommonEffectSetup[4].rotation = 0xC00;
    g_FxCommonEffectSetup[4].value30 = 0x24;
    g_FxCommonEffectSetup[4].enabled = (g_ScriptVariableBank[SCRIPT_VAR_FLAGS] >> 0x13) & 1;
    g_FxCommonEffectSetup[4].value22 = 0;
    g_FxCommonEffectSetup[4].value23 = 5;
    g_FxCommonEffectSetup[4].value28 = 0x10;
    g_FxCommonEffectSetup[5].variant = 1;
    g_FxCommonEffectSetup[4].value25 = 6;
    g_FxCommonEffectSetup[5].type = 2;
    g_FxCommonEffectSetup[5].motionIndices[0] = 0x11;
    g_FxCommonEffectSetup[5].subtype = 0;
    g_FxCommonEffectSetup[5].motionIndices[1] = 0x10;
    g_FxCommonEffectSetup[5].motionIndices[2] = 0x25;
    g_FxCommonEffectSetup[5].motionIndices[3] = 0x24;
    g_FxCommonEffectSetup[5].parameter38 = 0x1B58;
    g_FxCommonEffectSetup[5].parameter3c = 0xFA0;
    g_FxCommonEffectSetup[5].value28 = 0x30;
    g_FxCommonEffectSetup[5].value2c = -0x25;
    g_FxCommonEffectSetup[5].enabled = (g_ScriptVariableBank[SCRIPT_VAR_FLAGS] >> 0x17) & 1;
    g_FxCommonEffectSetup[5].value0c = 0x54;
    g_FxCommonEffectSetup[5].value10 = 0x161;
    g_FxCommonEffectSetup[5].value22 = 4;
    g_FxCommonEffectSetup[5].value23 = 7;
    g_FxCommonEffectSetup[5].rotation = 0;
    g_FxCommonEffectSetup[5].value25 = 0xA;
    g_FxCommonEffectSetup[6].subtype = 3;
    g_FxCommonEffectSetup[5].value30 = 0x1B;
    g_FxCommonEffectSetup[6].variant = 4;
    g_FxCommonEffectSetup[6].type = 5;
    g_FxCommonEffectSetup[6].motionIndices[0] = 0xD;
    g_FxCommonEffectSetup[6].motionIndices[1] = 0xC;
    g_FxCommonEffectSetup[6].motionIndices[2] = 0x21;
    g_FxCommonEffectSetup[6].motionIndices[3] = 0x20;
    g_FxCommonEffectSetup[6].value0c = 0x66;
    g_FxCommonEffectSetup[6].value28 = -0x60;
    g_FxCommonEffectSetup[6].value2c = 5;
    g_FxCommonEffectSetup[6].rotation = 0x800;
    g_FxCommonEffectSetup[6].value30 = 0x32;
    g_FxCommonEffectSetup[6].enabled = (g_ScriptVariableBank[SCRIPT_VAR_FLAGS] >> 0xE) & 1;
    g_FxCommonEffectSetup[6].value10 = 0x161;
    g_FxCommonEffectSetup[6].parameter38 = 0x1388;
    g_FxCommonEffectSetup[6].parameter3c = 0x9C4;
    g_FxCommonEffectSetup[6].value22 = 9;
    g_FxCommonEffectSetup[6].value23 = 3;
    g_FxCommonEffectSetup[6].value25 = 1;
    g_FxCommonEffectSetup[7].subtype = 0xC;
    g_FxCommonEffectSetup[7].variant = 0xD;
    g_FxCommonEffectSetup[7].type = 0xE;
    g_FxCommonEffectSetup[7].motionIndices[0] = 0x17;
    g_FxCommonEffectSetup[7].motionIndices[1] = 0x16;
    g_FxCommonEffectSetup[7].value10 = 0x161;
    g_FxCommonEffectSetup[7].motionIndices[2] = 0x2B;
    g_FxCommonEffectSetup[7].motionIndices[3] = 0x2A;
    g_FxCommonEffectSetup[7].value0c = 0x53;
    g_FxCommonEffectSetup[7].enabled = (g_ScriptVariableBank[SCRIPT_VAR_FLAGS] >> 0xF) & 1;
    g_FxCommonEffectSetup[7].parameter38 = 0x1388;
    g_FxCommonEffectSetup[7].parameter3c = 0x9C4;
    g_FxCommonEffectSetup[7].value22 = 5;
    g_FxCommonEffectSetup[7].value23 = 8;
    g_FxCommonEffectSetup[7].value28 = 0x20;
    g_FxCommonEffectSetup[7].value2c = -0x40;
    g_FxCommonEffectSetup[7].rotation = 0;
    g_FxCommonEffectSetup[7].value25 = 2;
    g_FxCommonEffectSetup[8].subtype = 0x12;
    g_FxCommonEffectSetup[7].value30 = 0xB;
    g_FxCommonEffectSetup[8].variant = 0x13;
    g_FxCommonEffectSetup[8].type = 0x14;
    g_FxCommonEffectSetup[8].motionIndices[0] = 0xF;
    g_FxCommonEffectSetup[8].motionIndices[1] = 0xE;
    g_FxCommonEffectSetup[8].motionIndices[2] = 0x23;
    g_FxCommonEffectSetup[8].motionIndices[3] = 0x22;
    g_FxCommonEffectSetup[8].value0c = 0x69;
    g_FxCommonEffectSetup[8].parameter38 = 0x4650;
    g_FxCommonEffectSetup[8].parameter3c = 0x2EE0;
    g_FxCommonEffectSetup[8].value10 = 0x161;
    g_FxCommonEffectSetup[8].value22 = 7;
    g_FxCommonEffectSetup[8].value23 = 9;
    g_FxCommonEffectSetup[8].value28 = -0x40;
    g_FxCommonEffectSetup[8].value2c = -0x30;
    g_FxCommonEffectSetup[8].rotation = 0;
    g_FxCommonEffectSetup[8].value30 = 0x35;
    g_FxCommonEffectSetup[8].enabled = (g_ScriptVariableBank[SCRIPT_VAR_FLAGS] >> 0x11) & 1;
    g_FxCommonEffectSetup[8].value25 = 4;
    g_FxCommonEffectSetup[9].subtype = 6;
    g_FxCommonEffectSetup[9].variant = 7;
    g_FxCommonEffectSetup[9].type = 8;
    g_FxCommonEffectSetup[9].value10 = 0x161;
    g_FxCommonEffectSetup[9].motionIndices[0] = 0x15;
    g_FxCommonEffectSetup[9].motionIndices[1] = 0x14;
    g_FxCommonEffectSetup[9].motionIndices[2] = 0x29;
    g_FxCommonEffectSetup[9].motionIndices[3] = 0x28;
    g_FxCommonEffectSetup[9].value0c = 0x59;
    g_FxCommonEffectSetup[9].value28 = -0x55;
    g_FxCommonEffectSetup[9].value2c = -0x15;
    g_FxCommonEffectSetup[9].rotation = 0x400;
    g_FxCommonEffectSetup[9].parameter38 = 0x1388;
    g_FxCommonEffectSetup[9].parameter3c = 0x9C4;
    g_FxCommonEffectSetup[9].value22 = 8;
    g_FxCommonEffectSetup[9].value23 = 6;
    g_FxCommonEffectSetup[9].value25 = 3;
    g_FxCommonEffectSetup[9].value30 = 0x5B;
    g_FxCommonEffectSetup[9].enabled = (g_ScriptVariableBank[SCRIPT_VAR_FLAGS] >> 0x10) & 1;
    for (; pointIndex < 10; ++pointIndex) {
        setupCursor->position[0] = pointList->x;
        *yCursor = (s16) (u16)pointList->y;
        pointZ = (u16)pointList->z;
        *zCursor = (s16) pointZ;
        firstInitialNode = FxCommon_CreateConfiguredNode(&g_FxCommonDefaultNode, *subtypeCursor, *variantCursor, *typeCursor, (void *) g_FxCommonEffectSetup[pointIndex].parameter38, (void *) g_FxCommonEffectSetup[pointIndex].parameter3c, Draw_GetTimTableEntry(&g_FxCommonModelTable, 0));
        initialSlots[pointIndex] = firstInitialNode;
        firstInitialNode->state[1] = setupCursor->position[0];
        (initialSlots[pointIndex])->state[2] = (s32) *yCursor;
        (initialSlots[pointIndex])->state[3] = (s32) *zCursor;
        initialNode = initialSlots[pointIndex];
        (initialNode)->value2E = (u16) *rotationCursor;
        initialResource = Draw_GetTimTableEntry(&g_FxCommonModelTable, 0);
        initialNodeType = *typeCursor;
        initialNodeVariant = *variantCursor;
        initialNodeSubtype = *subtypeCursor;
        secondInitialNode = FxCommon_CreateConfiguredNode(&g_FxCommonDefaultNode, initialNodeSubtype, initialNodeVariant, initialNodeType, (void *)0x3E8, (void *)0x1F4, initialResource);
        *secondInitialSlot = secondInitialNode;
        secondInitialNode->state[1] = setupCursor->position[0];
        (*secondInitialSlot)->state[2] = (s32) *yCursor;
        (*secondInitialSlot)->state[3] = (s32) *zCursor;
        (*secondInitialSlot)->value2E = (u16) *rotationCursor;
        pairedInitialNode = *secondInitialSlot;
        (pairedInitialNode)->value2C = 0x800;
        typeCursor += 26;
        variantCursor += 26;
        subtypeCursor += 26;
        yCursor += 26;
        zCursor += 26;
        rotationCursor += 26;
        secondInitialSlot++;
        pointList++;
        setupCursor++;
    }
    nodeResource = Draw_GetTimTableEntry(&g_FxCommonModelTable, 0);
    g_FxCommonRuntime.node0 = FxCommon_CreateBareNode(&g_FxCommonDefaultNode, Draw_GetTimTableEntry(&g_FxCommonModelTable, 1));
    g_FxCommonType51Node = FxCommon_CreateTypedNode(&g_FxCommonDefaultNode, 0x51, nodeResource);
    g_FxCommonSceneNodes[0] = FxCommon_CreateTypedNode(&g_FxCommonDefaultNode, 0x28, nodeResource);
    g_FxCommonSceneNodes[1] = FxCommon_CreateTypedNode(&g_FxCommonDefaultNode, 0x29, nodeResource);
    g_FxCommonSceneNodes[2] = FxCommon_CreateTypedNode(&g_FxCommonDefaultNode, 0x2A, nodeResource);
    g_FxCommonSceneNodes[3] = FxCommon_CreateTypedNode(&g_FxCommonDefaultNode, 0x2B, nodeResource);
    g_FxCommonSceneNodes[4] = FxCommon_CreateTypedNode(&g_FxCommonDefaultNode, 0x2C, nodeResource);
    g_FxCommonSceneNodes[5] = FxCommon_CreateTypedNode(&g_FxCommonDefaultNode, 0x2D, nodeResource);
    g_FxCommonSceneNodes[6] = FxCommon_CreateTypedNode(&g_FxCommonDefaultNode, 0x2E, nodeResource);
    g_FxCommonSceneNodes[7] = FxCommon_CreateTypedNode(&g_FxCommonDefaultNode, 0x2F, nodeResource);
    g_FxCommonSceneNodes[8] = FxCommon_CreateTypedNode(&g_FxCommonDefaultNode, 0x30, nodeResource);
    g_FxCommonSceneNodes[9] = FxCommon_CreateTypedNode(&g_FxCommonDefaultNode, 0x31, nodeResource);
    g_FxCommonSceneNodes[10] = FxCommon_CreateTypedNode(&g_FxCommonDefaultNode, 0x32, nodeResource);
    g_FxCommonRuntime.node25 = FxCommon_CreateTypedNode(&g_FxCommonDefaultNode, 0x25, nodeResource);
    g_FxCommonType54Node = FxCommon_CreateTypedNode(&g_FxCommonDefaultNode, 0x54, nodeResource);
    g_FxCommonPathTailNodes[0] = (FxCommonTransformNode *)FxCommon_CreateTypedNode(&g_FxCommonDefaultNode, 0x23, nodeResource);
    g_FxCommonPathTailNodes[1] = (FxCommonTransformNode *)FxCommon_CreateTypedNode(&g_FxCommonDefaultNode, 0x23, nodeResource);
    g_FxCommonPathNodes[0] = (FxCommonTransformNode *)FxCommon_CreateTypedNode(&g_FxCommonDefaultNode, 0x52, nodeResource);
    g_FxCommonPathNodes[2] = (FxCommonTransformNode *)FxCommon_CreateTypedNode(&g_FxCommonDefaultNode, 0x52, nodeResource);
    g_FxCommonPathNodes[1] = (FxCommonTransformNode *)FxCommon_CreateTypedNode(&g_FxCommonDefaultNode, 0x52, nodeResource);
    g_FxCommonPathNodes[3] = (FxCommonTransformNode *)FxCommon_CreateTypedNode(&g_FxCommonDefaultNode, 0x52, nodeResource);
    g_FxCommonEchoNodes[0] = (FxCommonTransformNode *)FxCommon_CreateTypedNode(&g_FxCommonDefaultNode, 0x53, nodeResource);
    g_FxCommonEchoNodes[2] = (FxCommonTransformNode *)FxCommon_CreateTypedNode(&g_FxCommonDefaultNode, 0x53, nodeResource);
    g_FxCommonEchoNodes[1] = (FxCommonTransformNode *)FxCommon_CreateTypedNode(&g_FxCommonDefaultNode, 0x53, nodeResource);
    g_FxCommonEchoNodes[3] = (FxCommonTransformNode *)FxCommon_CreateTypedNode(&g_FxCommonDefaultNode, 0x53, nodeResource);
    g_FxCommonType21Nodes[0] = FxCommon_CreateTypedNode(&g_FxCommonDefaultNode, 0x21, nodeResource);
    g_FxCommonType21Nodes[1] = FxCommon_CreateTypedNode(&g_FxCommonDefaultNode, 0x21, nodeResource);
    g_FxCommonType26Nodes[0] = FxCommon_CreateTypedNode(&g_FxCommonDefaultNode, 0x26, nodeResource);
    g_FxCommonType26Nodes[1] = FxCommon_CreateTypedNode(&g_FxCommonDefaultNode, 0x26, nodeResource);
    g_FxCommonType26Nodes[2] = FxCommon_CreateTypedNode(&g_FxCommonDefaultNode, 0x26, nodeResource);
    g_FxCommonType26Nodes[3] = FxCommon_CreateTypedNode(&g_FxCommonDefaultNode, 0x26, nodeResource);
    g_FxCommonLodNodes[0] = FxCommon_CreateConfiguredNode(&g_FxCommonDefaultNode, 0x22, 0x20, 0x20, (void *)0x2710, (void *)0x1F40, nodeResource);
    g_FxCommonLodNodes[1] = FxCommon_CreateConfiguredNode(&g_FxCommonDefaultNode, 0x22, 0x20, 0x20, (void *)0x14, (void *)0xA, nodeResource);
    g_FxCommonPairedNodes[0] = FxCommon_CreateConfiguredNode(&g_FxCommonDefaultNode, 0x22, 0x20, 0x20, (void *)0x2710, (void *)0x1F40, nodeResource);
    g_FxCommonPairedNodes[1] = FxCommon_CreateConfiguredNode(&g_FxCommonDefaultNode, 0x22, 0x20, 0x20, (void *)0x14, (void *)0xA, nodeResource);
    (g_FxCommonPairedNodes[0])->value2E = 0xC8;
    (g_FxCommonPairedNodes[1])->value2E = 0xC8;
    g_FxCommonPairedNodes[0]->state[1] = 0x334;
    g_FxCommonPairedNodes[0]->state[2] = 0x14;
    g_FxCommonPairedNodes[0]->state[3] -= 0xFA0;
    {
        FxCommonNode *node = g_FxCommonPairedNodes[1];
        node->state[1] = 0x334;
    }
    g_FxCommonPairedNodes[1]->state[2] = -0x14;
    g_FxCommonPairedNodes[1]->state[3] -= 0xFA0;
    g_FxCommonType26Nodes[0]->state[0] = -0x32C8;
    g_FxCommonType26Nodes[0]->state[1] = 0x1B58;
    g_FxCommonType26Nodes[0]->state[2] = -0x1770;
    g_FxCommonType26Nodes[1]->state[0] = -0x7D0;
    g_FxCommonType26Nodes[1]->state[1] = 0x2328;
    g_FxCommonType26Nodes[1]->state[2] = 0x7D0;
    g_FxCommonType26Nodes[2]->state[0] = 0x1388;
    g_FxCommonType26Nodes[2]->state[1] = 0x1770;
    g_FxCommonType26Nodes[2]->state[2] = 0xFA0;
    typeFixed = 0x350000;
    subtypeFixed = 0x330000;
    {
        FxCommonNode *node = g_FxCommonType26Nodes[3];
        /* The counter is initialized after this scheduling-only clobber. */
        asm volatile("" : : "r"(node) : "$23");
        groupIndex = 0;
        groupVariant = 0;
        node->state[0] = 0x2710;
    }
    {
        FxCommonNode *node = g_FxCommonType26Nodes[3];
        /* Keep the resource-offset constant after the final node loads. */
        asm("" : : "r"(subtypeFixed), "r"(node));
        resourceOffset = 4;
        node->state[1] = 0x1B58;
    }
    g_FxCommonType26Nodes[3]->state[2] = 0xBB8;
    for (; groupIndex < 9; ++groupIndex) {
        resourceData = Draw_GetTimTableEntry(resourceTableData, 2);
        resourceTable = (FxCommonOffsetTable *)&((u8 *)resourceData)[resourceOffset];
        pointOffset = resourceTable->offsets[0];
        pointIndex = 0;
        pointList = (FxCommonPoint8 *)&((u8 *)resourceData)[pointOffset];
        pointCount = ((FxCommonPointList *)pointList)->count;
        ++pointList;
        /* Separate capture preserves GCC 2.7.2's retail spill layout. */
        textureIndexOrHasPoints = pointIndex < pointCount;
        if (textureIndexOrHasPoints) {
            tableIndex = groupIndex * 2;
            point = pointList;
            variantBase = groupVariant;
            nodeSubtype = subtypeFixed >> 0x10;
            nodeType = typeFixed >> 0x10;
            do {
                effectRoot = &g_FxCommonDefaultNode;
                variantOrMotionOffset = variantBase + 0x34;
                firstNode = FxCommon_CreateConfiguredNode(effectRoot, (s16) nodeSubtype, variantOrMotionOffset, (s16) nodeType, (void *) (u16) *(short *)&((u8 *)g_FxCommonRouteFarDepths)[tableIndex], (void *) (u16) *(short *)&((u8 *)g_FxCommonRouteNearDepths)[tableIndex], nodeResource);
                firstSlot = &g_FxCommonRouteNodes[g_FxCommonRouteNodeCount];
                g_FxCommonRouteNodes[g_FxCommonRouteNodeCount] = firstNode;
                firstNode->state[1] = point->x;
                yNode = *firstSlot;
                yNode->state[2] = point->y;
                g_FxCommonRouteNodeCount += 1;
                (*firstSlot)->state[3] = point->z;
                secondNode = FxCommon_CreateConfiguredNode(effectRoot, (s16) nodeSubtype, variantOrMotionOffset, (s16) nodeType, (void *)0x7D0, (void *)0x1F4, nodeResource);
                nodeIndex = g_FxCommonRouteNodeCount;
                secondSlot = &g_FxCommonRouteNodes[nodeIndex];
                g_FxCommonRouteNodes[nodeIndex] = secondNode;
                secondNode->state[1] = point->x;
                (*secondSlot)->state[2] = point->y;
                nextPointIndex = ++pointIndex;
                (*secondSlot)->state[3] = point->z;
                pairedNode = *secondSlot;
                asm("" : "=r"(nodeIndex), "=r"(point) : "0"(nodeIndex), "1"(point), "r"(pointList), "r"(pairedNode));
                g_FxCommonRouteNodeCount = nodeIndex + 1;
                /* Byte view retains the retail ordering after the pool-index store. */
                *(u16 *)&((u8 *)pairedNode)[PE1_OFFSETOF(FxCommonNode, value2C)] =
                    (u16) (*(u16 *)&((u8 *)pairedNode)[PE1_OFFSETOF(FxCommonNode, value2C)]
                           + 0x800);
                point++;
            } while (nextPointIndex < pointCount);
        }
        typeFixed += 0x30000;
        subtypeFixed += 0x30000;
        groupVariant += 3;
        resourceOffset += 4;
    }
    g_FxCommonRuntime.node25->state[0] = 0;
    g_FxCommonRuntime.node25->state[1] = 0x758;
    g_FxCommonRuntime.node25->state[2] = 0;
    FxCommon_StartCameraMove(g_FxCommonRoomVariant, 1, 1, 0);
    motionIndex = 0;
    g_FxCommonMotionBlendFrames = 0;
    motionBase = (s16) (u16) g_FxCommonRoomVariant;
    /* The earlier variant is dead; the motion byte offset is assigned below. */
    asm("" : : "r"((u16)motionBase) : "$16");
    variantOrMotionOffset = 0;
    for (; motionIndex < 10; ++motionIndex) {
        FxCommon_SamplePath(motionIndex << 8, motionBase + 0x40, Draw_GetTimTableEntry(resourceTableData, 2), &motion.vector, &shortMotion);
        *((s32 *)&((u8 *)g_FxCommonMotionPositions)[variantOrMotionOffset]) = motion.vector.x << 0x10;
        *((s32 *)&((u8 *)g_FxCommonMotionPositionY)[variantOrMotionOffset]) = motion.vector.y << 0x10;
        *((s32 *)&((u8 *)g_FxCommonMotionPositionZ)[variantOrMotionOffset]) = motion.vector.z << 0x10;
        variantOrMotionOffset += 0x10;
    }
    if (g_FxCommonAutoPlay == 1) {
        g_FxCommonMarkerFade = 0;
        g_FxCommonPlayAdvance = 1;
        g_FxCommonPlaying = 1;
        g_FxCommonPlayTicks = 1;
        g_FxCommonRoomVariant = (s16) g_FxCommonAutoVariant;
    }
    if (g_FxCommonStartPreset == 0) {
        g_FxCommonRoomVariant = 7;
        g_FxCommonPlayFrame = 0;
        g_FxCommonPlayAdvance = 0;
        g_FxCommonReplayCamera = 0;
        g_FxCommonPlayTicks = 0;
        g_FxCommonPlaying = 0;
        g_FxCommonMarkerFade = 0;
        g_FxCommonStartPreset0ClearedWriteOnly = 0;
        g_FxCommonViewMode = 0xA;
        FxCommon_StartCameraMove(7, 0, 0, 0);
        g_FxCommonStartPreset0LatchWriteOnly = 1;
    }
    if (g_FxCommonStartPreset == 7) {
        selectedMotion = g_FxCommonStartCameraPath;
        g_FxCommonRoomVariant = 9;
        g_FxCommonPlayFrame = 0;
        g_FxCommonMarkerFade = 0;
        g_FxCommonPlayAdvance = 0;
        g_FxCommonPlaying = 1;
        g_FxCommonPlayTicks = 1;
        g_FxCommonAutoPlay = 1;
        FxCommon_SamplePath(0, selectedMotion + 1, Draw_GetTimTableEntry(resourceTableData, 2), &motion.vector, &shortMotion);
        /* Shared volatile is scheduling debt in other callers; this store is ordinary. */
        *(s16 *)&g_FxCommonMotionRemaining0 = 0;
        g_FxCommonCameraTarget.x = motion.vector.x;
        g_FxCommonCameraTarget.y = motion.vector.y;
        g_FxCommonCameraTarget.z = motion.vector.z;
        motionCount = FxCommon_SamplePath(0, selectedMotion, Draw_GetTimTableEntry(resourceTableData, 2), &motion.vector, &shortMotion) - 2;
        g_FxCommonMotionRemaining1 = 0;
        g_FxCommonCameraPathRemaining = motionCount;
        g_FxCommonCameraPathStep = 0;
        g_FxCommonCameraPathIndex = selectedMotion;
        g_FxCommonMarkerFade = 0;
        g_FxCommonCameraPosition.x = motion.vector.x;
        g_FxCommonCameraPosition.y = motion.vector.y;
        g_FxCommonCameraPosition.z = motion.vector.z;
    }
}
