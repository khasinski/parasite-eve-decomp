/* CC1_FLAGS: -G0 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
#include "pe1/battle_runtime.h"
#include "pe1/render_object.h"

u8 g_BattleFadeLevel;
s8 D_8009CE6C;

/* Fade the targeted actor; group members (kind 4) fade together, linked parts
 * (kind 1) fade their parent. The level is evaluated at the call. */
#define FADE_TARGET(target, level)                                                       \
    {                                                                                    \
        BattleEntity *entity = (target);                                                 \
        BattleEntity *actor;                                                             \
        int kind = (s8)((Combatant *)entity->core)->field04.bytes.field05;               \
        if (kind == 1) {                                                                 \
            Render_FadeEntityColor(&entity->parent->renderObject, (level), (level),      \
                                   (level));                                             \
        } else if (kind == 4) {                                                          \
            for (actor = D_8009D20C; actor != 0; actor = actor->next) {                  \
                if (actor != D_8009D254 && actor->core != 0 &&                           \
                    (s8)((Combatant *)actor->core)->field04.bytes.field05 == 4) {        \
                    Render_FadeEntityColor(&actor->renderObject, g_BattleFadeLevel,      \
                                           g_BattleFadeLevel, g_BattleFadeLevel);        \
                }                                                                        \
            }                                                                            \
        } else {                                                                         \
            Render_FadeEntityColor(&entity->renderObject, (level), (level), (level));    \
        }                                                                                \
    }

void Battle_StepPlayerTurn(BattleTarget *targets, int index, int mode)
{
    u8 level;
    s16 center;
    s16 lower;
    s16 upper;
    s16 angle;
    int center_copy, low_copy, high_copy, inside;
    u8 slot;
    int targetMode;

    if (D_8009D2B0 == 0) {
        return;
    }
    if (g_BattleFadeLevel < 0x41) {
        D_8009CE6C = 8;
    } else if (g_BattleFadeLevel >= 0xC0) {
        D_8009CE6C = -8;
    }
    level = g_BattleFadeLevel + D_8009CE6C;
    g_BattleFadeLevel = level;

    if ((s8)mode < 4) {
        targetMode = (D_8009D278->action->turnWord >> 6) & 3;
        switch (targetMode) {
        case 0:
            FADE_TARGET(targets[(s8)index].actor, level);
            break;
        case 2:
            center = targets[(s8)index].angle;
            if (center < -0x600) {
                lower = center + 0x200;
                upper = center + 0xE00;
            } else if (center < 0x600) {
                lower = center - 0x200;
                upper = center + 0x200;
            } else {
                lower = center - 0xE00;
                upper = center - 0x200;
            }
            if (targets[0].actor == 0) {
                return;
            }
            slot = 0;
            high_copy = upper;
            low_copy = lower;
            center_copy = center;
            inside = center < 0x600;
            for (; targets[slot].actor != 0; slot++) {
                if (center_copy >= -0x600 && inside) {
                    angle = targets[slot].angle;
                    if (angle < low_copy || angle > high_copy) {
                        continue;
                    }
                } else {
                    angle = targets[slot].angle;
                    if (angle < high_copy && angle > low_copy) {
                        continue;
                    }
                }
                FADE_TARGET(targets[slot].actor, g_BattleFadeLevel);
            }
            break;
        case 1:
        case 3:
            for (slot = 0; targets[slot].actor != 0; slot++) {
                FADE_TARGET(targets[slot].actor, g_BattleFadeLevel);
            }
            break;
        }
    } else if ((s8)mode < 8) {
        FADE_TARGET(targets[(s8)index].actor, level);
    }
}
