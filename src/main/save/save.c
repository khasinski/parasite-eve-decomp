#include "include_asm.h"

typedef signed char s8;
typedef unsigned char u8;
typedef unsigned int u32;

#include "pe1/save_blob.h"

extern SaveBytes800 g_EntityWorkBuffer;
extern SaveBytes70 g_AyaBattleState;
extern SaveBytes18 g_SavedBattleStateTail;
extern SaveBytes8 g_BattleEquipStateBlock;
extern SaveBytes4 g_FieldMoveLock, g_SceneDispatchToken, g_GameStateFlags, D_800B0CDC;
extern s8 D_800B0CE0, g_LoadedTexturePageId, g_SceneAreaType, g_SavedSceneAreaType, D_800B0CE4, g_PendingDiscSide;
extern u8 g_DiscChangeFlags, g_ScreenTransitionState;
extern u8 *g_SaveIoCursor;

extern volatile int g_SaveTitleStyleFlag;

void Save_SerializeTail(void) {
    register u8 *cursor asm("$3");
    register u8 *cd asm("$7");
    register u8 *t asm("$2");

    cd = g_SaveIoCursor;
    *(SaveBytes800 *)cd = g_EntityWorkBuffer;

    cursor = g_SaveIoCursor; g_SaveIoCursor = cursor + 0x800; *(SaveBytes4 *)(cursor + 0x800) = g_FieldMoveLock;
    cursor = g_SaveIoCursor; g_SaveIoCursor = cursor + 4; *(SaveBytes4 *)(cursor + 4) = g_SceneDispatchToken;
    cursor = g_SaveIoCursor; g_SaveIoCursor = cursor + 4; *(SaveBytes4 *)(cursor + 4) = g_GameStateFlags;
    cursor = g_SaveIoCursor; g_SaveIoCursor = cursor + 4; *(SaveBytes4 *)(cursor + 4) = D_800B0CDC;
    __asm__ volatile(
    ".set push\n"
    ".set noreorder\n"
    ".set noat\n"
    "lui $v1, %%hi(g_SaveIoCursor)\n"
    "lw $v1, %%lo(g_SaveIoCursor)($v1)\n"
    "nop\n"
    "addiu $v0, $v1, 4\n"
    "lui $at, %%hi(g_SaveIoCursor)\n"
    "sw $v0, %%lo(g_SaveIoCursor)($at)\n"
    "lui $v0, %%hi(D_800B0CE0)\n"
    "lb $v0, %%lo(D_800B0CE0)($v0)\n"
    "lui $a0, %%hi(g_LoadedTexturePageId)\n"
    "lb $a0, %%lo(g_LoadedTexturePageId)($a0)\n"
    "sb $v0, 4($v1)\n"
    "sb $a0, 5($v1)\n"
    "lui $v1, %%hi(g_SaveIoCursor)\n"
    "lw $v1, %%lo(g_SaveIoCursor)($v1)\n"
    "nop\n"
    "addiu $v0, $v1, 2\n"
    "lui $at, %%hi(g_SaveIoCursor)\n"
    "sw $v0, %%lo(g_SaveIoCursor)($at)\n"
    "lui $v0, %%hi(g_SceneAreaType)\n"
    "lb $v0, %%lo(g_SceneAreaType)($v0)\n"
    "lui $a0, %%hi(g_SavedSceneAreaType)\n"
    "lb $a0, %%lo(g_SavedSceneAreaType)($a0)\n"
    "sb $v0, 2($v1)\n"
    "sb $a0, 3($v1)\n"
    "lui $v1, %%hi(g_SaveIoCursor)\n"
    "lw $v1, %%lo(g_SaveIoCursor)($v1)\n"
    "nop\n"
    "addiu $v0, $v1, 2\n"
    "lui $at, %%hi(g_SaveIoCursor)\n"
    "sw $v0, %%lo(g_SaveIoCursor)($at)\n"
    "lui $v0, %%hi(D_800B0CE4)\n"
    "lb $v0, %%lo(D_800B0CE4)($v0)\n"
    "lui $a0, %%hi(g_PendingDiscSide)\n"
    "lb $a0, %%lo(g_PendingDiscSide)($a0)\n"
    "sb $v0, 2($v1)\n"
    "sb $a0, 3($v1)\n"
    "lui $v1, %%hi(g_SaveIoCursor)\n"
    "lw $v1, %%lo(g_SaveIoCursor)($v1)\n"
    "lui $a0, %%hi(g_DiscChangeFlags)\n"
    "lbu $a0, %%lo(g_DiscChangeFlags)($a0)\n"
    "addiu $v0, $v1, 2\n"
    "lui $at, %%hi(g_SaveIoCursor)\n"
    "sw $v0, %%lo(g_SaveIoCursor)($at)\n"
    "sb $a0, 2($v1)\n"
    "lui $v1, %%hi(g_SaveIoCursor)\n"
    "lw $v1, %%lo(g_SaveIoCursor)($v1)\n"
    "nop\n"
    "addiu $v0, $v1, 1\n"
    "lui $at, %%hi(g_SaveIoCursor)\n"
    "sw $v0, %%lo(g_SaveIoCursor)($at)\n"
    "lui $v0, %%hi(g_ScreenTransitionState)\n"
    "lbu $v0, %%lo(g_ScreenTransitionState)($v0)\n"
    "nop\n"
    "sb $v0, 1($v1)\n"
    ".set pop\n"
    ::: "memory", "$2", "$3", "$4");
    t = g_SaveIoCursor + 1; cd = t; g_SaveIoCursor = cd; *(SaveBytes70 *)cd = g_AyaBattleState;
    cursor = g_SaveIoCursor; g_SaveIoCursor = cursor + 0x70; *(SaveBytes18 *)(cursor + 0x70) = g_SavedBattleStateTail;
    cursor = g_SaveIoCursor; g_SaveIoCursor = cursor + 0x18; *(SaveBytes8 *)(cursor + 0x18) = g_BattleEquipStateBlock;
    t = g_SaveIoCursor; g_SaveIoCursor = t + 8;
}

void Save_DeserializeTail(void) {
    u8 *cursor;
    u8 *next;

    cursor = g_SaveIoCursor;
    g_EntityWorkBuffer = *(SaveBytes800 *)cursor;

    cursor = g_SaveIoCursor;
    g_SaveIoCursor = cursor + 0x800;
    g_FieldMoveLock = *(SaveBytes4 *)(cursor + 0x800);

    cursor = g_SaveIoCursor;
    g_SaveIoCursor = cursor + 4;
    g_SceneDispatchToken = *(SaveBytes4 *)(cursor + 4);

    cursor = g_SaveIoCursor;
    g_SaveIoCursor = cursor + 4;
    g_GameStateFlags = *(SaveBytes4 *)(cursor + 4);

    cursor = g_SaveIoCursor;
    g_SaveIoCursor = cursor + 4;
    D_800B0CDC = *(SaveBytes4 *)(cursor + 4);

    cursor = g_SaveIoCursor;
    g_SaveIoCursor = cursor + 4;
    D_800B0CE0 = cursor[4];
    g_LoadedTexturePageId = cursor[5];

    cursor = g_SaveIoCursor;
    g_SaveIoCursor = cursor + 2;
    g_SceneAreaType = cursor[2];
    g_SavedSceneAreaType = cursor[3];

    cursor = g_SaveIoCursor;
    next = cursor + 4;
    g_SaveIoCursor = cursor + 2;
    D_800B0CE4 = cursor[2];

    g_SaveIoCursor = cursor + 3;
    g_PendingDiscSide = cursor[3];

    g_DiscChangeFlags = cursor[2];
    g_ScreenTransitionState = cursor[3];
    g_SaveIoCursor = next;

    cursor = g_SaveIoCursor;
    g_AyaBattleState = *(SaveBytes70 *)cursor;

    cursor = g_SaveIoCursor;
    g_SaveIoCursor = cursor + 0x70;
    g_SavedBattleStateTail = *(SaveBytes18 *)(cursor + 0x70);

    cursor = g_SaveIoCursor;
    g_SaveIoCursor = cursor + 0x18;
    g_BattleEquipStateBlock = *(SaveBytes8 *)(cursor + 0x18);

    cursor = g_SaveIoCursor;
    g_SaveIoCursor = cursor + 8;

    *(u32 *)&g_GameStateFlags &= 0xFFFF2679;
    *(u32 *)&g_FieldMoveLock &= 0xFFFFFFF6;
}

void Save_SetTitleStyleFlag(int value) {
    g_SaveTitleStyleFlag = value;
}
