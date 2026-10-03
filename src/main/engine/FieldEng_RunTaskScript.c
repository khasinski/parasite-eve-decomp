#include "pe1/field_anim.h"
#include "pe1/field_actor.h"
#include "pe1/render_object.h"
#include "pe1/pe_image.h"

/* Runs the owner's task script until it yields, then steps every live task.
 * Opcodes: 0 stop (end of script), 1 set variable, 2 spawn task,
 * 3 count down a variable and jump while positive, 4 wait on a timer or
 * a variable, 5 toggle the owner's held state. */
int func_800D413C(FieldAnimTaskOwner *owner)
{
    FieldAnimTaskContext *context = &owner->tasks;
    FieldAnimTaskSlot *slot;
    FieldActorState *state;
    unsigned int flags;
    int count;
    int running = 1;
    int result;
    int i;
    s16 op, a, b;

    D_800F32D0 = &owner->prefix;
    D_800E2368 = context;
    do {
        if (context->delay != 0) {
            if (--context->delay != 0)
                goto check_owner;
            context->delay = 0;
        }
        op = *context->pc.script;
        context->pc.script++;
        a = *context->pc.script;
        context->pc.script++;
        b = *context->pc.script;
        context->pc.script++;
        switch (op) {
        case 0:
            context->pc.script -= 3;
            if (context->count == 0) {
                owner->prefix.reserved00 = 4;
                if (D_800E2368->flags) {
                    if (D_800F32D0->actor->state != 0) {
                        D_800F32D0->actor->state->core_flags &= 0xC0FFFFFF;
                        *D_800F32D0->actor->state->action_state = 4;
                        D_800E2368->flags = 0;
                    }
                }
            }
            running = 0;
            break;
        case 1:
            context->variables[a] = b;
            break;
        case 2:
            func_800D401C(a);
            break;
        case 3:
            if ((s16)--context->variables[b] <= 0) {
                context->variables[b] = 0;
                running = 0;
            } else {
                /* Retail takes the operand as an absolute address. */
                context->pc.address = a - 6;
            }
            break;
        case 4:
            if (b != 1) {
                context->delay = a;
                running = 0;
            } else if (context->variables[a] == 0) {
                running = 0;
                context->pc.script -= 3;
            }
            break;
        case 5:
            if (a == 0) {
                D_800F32D0->actor->state->core_flags =
                    (D_800F32D0->actor->state->core_flags & 0xC0FFFFFF) | 0x1000000;
                D_800E2368->flags = 1;
            } else if (D_800E2368->flags && D_800F32D0->actor && D_800F32D0->actor->state) {
                u8 *mode = D_800F32D0->actor->state->action_state;
                if (*mode == 1)
                    *mode = 2;
            }
            break;
        }
    } while (running);
check_owner:
    if (D_800E2368->flags && D_800F32D0->actor && D_800F32D0->actor->state) {
        state = D_800F32D0->actor->state;
        flags = state->core_flags;
        count = (flags >> 24) & 0x3F;
        if (count >= 2)
            state->core_flags = (flags & 0xC0FFFFFF) | (((count - 1) & 0x3F) << 24);
        if ((int)((state->core_flags >> 24) & 0x3F) > 0) {
            if (*state->action_state != 0)
                goto step_tasks;
            if (state->control10.command_value <= 0)
                owner->prefix.reserved00 = 4;
        }
        if (*state->action_state == 0) {
            if (state->core_flags & 0x180E)
                owner->prefix.reserved00 = 4;
            if (D_800F32D0->actor->mode < 2)
                return 0;
        }
    }
step_tasks:
    if (context->flags)
        D_800F3428 = Asset_SearchByKeyType(owner->prefix.asset_type);
    slot = context->slots;
    for (i = 0; i < 8; i++, slot++) {
        if (slot->id != 0xFFFF) {
            D_800F33E0 = slot;
            D_800E27EC = slot->age;
            result = context->table->callbacks[slot->id](1, slot->start, context->argument);
            if (slot->end) {
                if (result == 2) {
                    if (!func_800CE688(slot->end))
                        result = 1;
                } else {
                    func_800CE688(slot->end);
                }
            }
            slot->age++;
            if (result == 1) {
                slot->id = 0xFFFF;
                context->count--;
            }
        }
    }
    return 0;
}
