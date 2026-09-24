/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
#include "pe1/battle_runtime.h"

extern struct { Combatant *value; char pad[12]; } D_8009D278_absolute __asm__("D_8009D278");
extern struct { BattleEntity *value; char pad[12]; } D_8009D20C_absolute __asm__("D_8009D20C");
extern struct { BattleEntity *value; char pad[12]; } D_8009D254_absolute __asm__("D_8009D254");
extern struct { u8 value; char pad[15]; } D_8009D1D4_absolute __asm__("D_8009D1D4");
extern struct { u8 value; char pad[15]; } D_8009D23C_read __asm__("D_8009D23C");
extern struct { u8 value; char pad[15]; } D_8009D23C_write __asm__("D_8009D23C");
extern struct { s32 value; char pad[12]; } D_8009D208_absolute __asm__("D_8009D208");
extern u8 D_8009CE68;
extern BattleEntity *D_800B8A90[];
#define INDEX (D_8009D1D4_absolute.value)
#define QUEUE_COUNT_READ (D_8009D23C_read.value)
#define QUEUE_COUNT_WRITE (D_8009D23C_write.value)
#define LOAD_RESULT (D_8009D208_absolute.value)
typedef struct BattleActionSlotCore {
    u32 flags;
    u8 rank;
    s8 mode;
    u8 pad_06[10];
    s32 field10;
} BattleActionSlotCore;
PE1_STATIC_ASSERT(PE1_OFFSETOF(BattleActionSlotCore, field10) == 0x10, battle_action_slot_core_field10);

int Battle_ProcessActionSlot(BattleEntity *entity)
{
    register BattleEntity *actor asm("$7") = entity;
    /* Fix the entity pointer in a3 before the prologue saves registers. */
    asm volatile("" : : "r"(actor));
    {
        register BattleActionSlotCore *state asm("$17") = (BattleActionSlotCore *)actor->core;
        int phase = state->flags & 0x6000;
        int result = 1;

        if (phase == 0x2000) {
            int index = INDEX;
            s16 command = D_800BE830[index].field04;
            if (command != 0x196 && command != 0x189) {
                if (D_8009D278_absolute.value->action->actionCode.actionId != 6) {
                    if (QUEUE_COUNT_READ == 0 && D_800B8A90[0] != 0) {
                        int i = 0;
                        do {
                            D_800B8A90[(u8)i] = 0;
                            i++;
                        } while (D_800B8A90[(u8)i] != 0);
                    }
                    {
                        u8 count = QUEUE_COUNT_READ;
                        QUEUE_COUNT_WRITE = count + 1;
                        D_800B8A90[count] = actor;
                    }
                } else {
                    register int room asm("$4") = 0x5B;
                    if (actor != D_800BE830[index].actor) goto after_room;
                    LOAD_RESULT = Scene_LoadRoomAssets(room, actor);
                    QUEUE_COUNT_WRITE = 0;
                after_room:;
                }
            }
            result = 0;
            if ((s8)state->mode == 0 || state->field10 <= 0) goto done;
            D_8009CE68 = 0xFF;
            goto done;
        }
        if (phase != 0x4000 || (s8)state->mode == 0) goto done;

        {
            register int one asm("$2") = 1;
            u8 color = D_8009CE68 - 8;
            D_8009CE68 = color;
            if ((s8)state->mode == one) {
                if (color <= 0x80) {
                    actor->parent->renderObject.flags_9C |= 0x20;
                    goto clear_state;
                } else {
                    Render_FadeEntityColor(&actor->parent->renderObject, color, color, color);
                    result = 0;
                    goto done;
                }
            }
            if ((s8)state->mode == 4) {
                BattleEntity *iter = D_8009D20C_absolute.value;
                for (; iter != 0; iter = iter->next) {
                    if (iter != D_8009D254_absolute.value && iter->core != 0 &&
                        ((BattleActionSlotCore *)iter->core)->mode == 4) {
                        int shade = D_8009CE68;
                        if ((u8)shade <= 0x80) {
                            iter->renderObject.flags_9C |= 0x20;
                            state->flags &= ~0x6000;
                        } else {
                            Render_FadeEntityColor(&iter->renderObject, shade, shade, shade);
                            result = 0;
                        }
                        /* Preserve the core flag reload across iterations. */
                        asm volatile("" : : : "memory");
                    }
                }
                goto done;
            }
            if (state->field10 <= 0) goto done;
            if (color <= 0x80) {
                actor->renderObject.flags_9C |= 0x20;
            clear_state:
                state->flags &= ~0x6000;
                goto done;
            }
            {
                register int green asm("$6") = color;
                /* Keep the green channel in a2 at the call boundary. */
                asm volatile("" : "=r"(green) : "0"(green));
                Render_FadeEntityColor(&actor->renderObject, color, green, color);
                result = 0;
            }
        }
    done:
        return result;
    }
}
