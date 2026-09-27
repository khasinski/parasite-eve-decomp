/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
#include "pe1/battle.h"
extern BattleInitSlot D_800BE830[45];
extern Combatant *D_8009D278[];
extern u8 D_8009D2B0[];
extern u8 D_8009CE3C, D_8009CE44, D_8009CE60;
extern u8 D_8009D2D8, D_8009D1DC;
void BattleCmd_UndoPending(void);

/* Matching debt: the empty memory barriers preserve repeated slot-index
 * reads; the register barriers keep the two entry calculations distinct.
 * RewindTurn retains one explicit loop-entry jump. No instruction ASM. */
static inline void RewindTurn(int initial)
{
    int next = initial - 1;
    goto store_index;
    do {
        asm volatile("" : "=m"(D_8009CE3C) : "m"(D_8009CE3C));
        next = D_8009CE3C - 1;
store_index:
        D_8009CE3C = next;
    } while (D_8009CE3C != 0 &&
        D_800BE830[D_8009CE3C].field06 ==
        D_800BE830[D_8009CE3C - 1].field06);
}

static inline void RewindEqual(int initial)
{
    int current = initial - 1;
    D_8009CE3C = current;
    if ((u8)current == 0 ||
        D_800BE830[(u8)current].field06 !=
        D_800BE830[(u8)current - 1].field06)
        return;
    do {
        asm volatile("" : "=m"(D_8009CE3C) : "m"(D_8009CE3C));
        D_8009CE3C--;
    } while (D_8009CE3C != 0 &&
        D_800BE830[D_8009CE3C].field06 ==
        D_800BE830[D_8009CE3C - 1].field06);
}

static inline void RestorePending(int index)
{
    if (D_8009CE60) {
        D_8009CE60 = 0;
        D_8009CE3C = index;
    }
}

void Battle_AdvanceTurnSlot(void)
{
    int savedIndex = D_8009CE3C;
    int kind;
    s16 i;
    s8 generation;
    D_8009CE44 = 0;
    kind = D_800BE830[savedIndex - 1].field04;
    if (kind == 1) {
        asm volatile("" : "=r"(savedIndex) : "0"(savedIndex));
        if (D_8009D1DC == (D_8009D278[0]->action->turnWord & 15)) {
            D_8009D2D8++;
            RewindEqual(savedIndex);
            RestorePending(savedIndex);
        } else {
            asm volatile("" : "=r"(savedIndex) : "0"(savedIndex));
            RewindTurn(savedIndex);
        }
        D_8009D1DC = D_8009D278[0]->action->turnWord & 15;
    } else if (kind == 2) {
        int mode = D_8009D278[0]->action->turnWord & 0xC0;
        if (mode == 0xC0) {
            D_8009D2D8++;
            D_8009CE3C = savedIndex - D_8009D2B0[0];
        } else if (mode == 0x40) {
            int word;
            D_8009D2D8++;
            word = D_8009D278[0]->action->turnWord;
            D_8009CE3C = savedIndex - (((word & 15) * 3) >> 1);
        }
        D_8009D1DC = D_8009D278[0]->action->turnWord & 15;
    } else if (kind < 0x197) {
        D_8009D2D8++;
        if (!D_8009CE60)
            BattleCmd_UndoPending();
        savedIndex = D_8009CE3C;
        {
            u8 current;
            D_8009CE3C = savedIndex - 1;
            for (;;) {
                asm volatile("" : "=m"(D_8009CE3C) : "m"(D_8009CE3C));
                current = D_8009CE3C;
                if (current == 0 ||
                    D_800BE830[current].field06 != D_800BE830[current - 1].field06)
                    break;
                D_8009CE3C = current - 1;
            }
        }
        RestorePending(savedIndex);
        D_8009D1DC = D_8009D278[0]->action->turnWord & 15;
    }
    generation = D_8009D2D8;
    for (i = 0; i < 45; i++) {
        if (D_800BE830[i].field06 == generation) {
            D_800BE830[i].actor = 0;
            (&D_800BE830[i])->field06 = 0;
            D_800BE830[i].field04 = 0;
        }
    }
}
