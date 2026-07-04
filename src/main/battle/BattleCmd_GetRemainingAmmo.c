/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

#include "pe1/battle_cmd.h"

/* Incomplete array: retail addresses this in-window pointer absolutely,
 * so it must not become -G8 small data. */
extern void **g_PlayerEntity[];
extern BattleCmdEntry g_BattleCmdStack[];
extern BattleCmdEntry *g_BattleCmdStackTop;

int BattleCmd_GetRemainingAmmo(int *out) {
    BattleCmdEntry *cursor;
    void **entity;
    void *current;
    int remaining = 0;

    entity = g_PlayerEntity[0];
    if (entity != 0) {
        if (entity[0] != 0) {
            current = entity[0];
            remaining = *(short *)((char *)current + 0xA);
            cursor = g_BattleCmdStack;

            while (cursor < g_BattleCmdStackTop) {
                /* retail compares the whole first word, not the opcode byte */
                if (*(int *)cursor == 1) {
                    remaining -= cursor->arg_08;
                }
                cursor++;
            }

            if (out != 0) {
                *out = *(short *)((char *)current + 0x2A);
            }
        }
    }

    return remaining;
}
