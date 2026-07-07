/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
typedef signed char s8;typedef unsigned char u8;typedef short s16;typedef unsigned short u16;typedef int s32;typedef unsigned int u32;typedef long long s64;
#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"

typedef struct {
    s32 words[28];
} AyaBattleState;

typedef struct {
    s32 words[6];
} BattleStateTail;

extern AyaBattleState D_80010928;
extern BattleStateTail D_80010998;
extern s32 g_BattleEquipStateBlock;
extern s32 D_8009D1B4[];
#define D_8009D1B4 (D_8009D1B4[0])
extern BattleStateTail g_SavedBattleStateTail;
extern AyaBattleState g_AyaBattleState;

void Battle_SaveAyaState(void) {
    struct {
        AyaBattleState aya;
        BattleStateTail tail;
        s32 zero0;
        s32 zero1;
    } state;

    state.aya = D_80010928;
    state.tail = D_80010998;
    state.zero0 = 0;
    state.zero1 = 0;
    g_AyaBattleState = state.aya;
    g_SavedBattleStateTail = state.tail;
    asm volatile("" ::: "memory");
    {
        volatile s32 *zeros = &state.zero0;
        g_BattleEquipStateBlock = zeros[0];
        D_8009D1B4 = zeros[1];
    }
}
