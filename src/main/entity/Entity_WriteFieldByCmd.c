#include "common.h"
#include "pe1/field_actor.h"
void Entity_WriteFieldByCmd(FieldActor *arg0, s32 arg1, s32 arg2) {
    s32 temp_a1;
    FieldActorState *temp_a0;

    temp_a0 = arg0->state;
    temp_a1 = arg1 & 0xFF;
    switch (temp_a1) {                              /* switch 1 */
    case 0x28:                                      /* switch 1 */
        temp_a0->amount_mirror = arg2;
        return;
    case 0x29:                                      /* switch 1 */
        temp_a0->id04.bytes.low = arg2;
        return;
    case 0x2A:                                      /* switch 1 */
        temp_a0->id04.bytes.high = arg2;
        return;
    case 0x2C:                                      /* switch 1 */
        temp_a0->control10.command_value = arg2;
        return;
    case 0x2D:                                      /* switch 1 */
        temp_a0->action_value14 = arg2;
        return;
    case 0x2B:                                      /* switch 1 */
        temp_a0->amount = arg2;
        return;
    case 0x2E:                                      /* switch 1 */
        temp_a0->id06.bytes.high = arg2;
        return;
    case 0x2F:                                      /* switch 1 */
        temp_a0->id06.bytes.low = arg2;
        return;
    case 0x3C:                                      /* switch 1 */
        temp_a0->script_value88 = arg2;
        return;
    case 0x3D:                                      /* switch 1 */
        temp_a0->script_value8c = arg2;
        return;
    case 0x3E:                                      /* switch 1 */
        temp_a0->script_value8e = arg2;
        return;
    case 0x3F:                                      /* switch 1 */
        temp_a0->effect_chance = arg2;
        return;
    case 0x40:                                      /* switch 1 */
        temp_a0->effect_param91 = arg2;
        return;
    case 0x41:                                      /* switch 1 */
        temp_a0->effect_param92 = arg2;
        return;
    case 0x42:                                      /* switch 1 */
        temp_a0->effect_param93 = arg2;
        return;
    case 0x43:                                      /* switch 1 */
        temp_a0->effect_level = arg2;
        return;
    case 0x44:                                      /* switch 1 */
        temp_a0->effect_duration = arg2;
        return;
    case 0x45:                                      /* switch 1 */
        temp_a0->script_value96 = arg2;
        return;
    case 0x4A:                                      /* switch 1 */
        temp_a0->core_flags = (temp_a0->core_flags & ~0x10) | ((arg2 & 1) * 0x10);
        return;
    case 0x50:                                      /* switch 1 */
        temp_a0->behavior_mode = arg2;
        switch (arg2) {                             /* switch 2; irregular */
        case 1:                                     /* switch 2 */
            temp_a0->behavior_timer = 0x190;
            return;
        case 2:                                     /* switch 2 */
            temp_a0->behavior_timer = 0x46;
            return;
        case 3:                                     /* switch 2 */
            temp_a0->behavior_timer = 0x14;
            return;
        }
        break;
    case 0x51:                                      /* switch 1 */
        temp_a0->core_flags = (temp_a0->core_flags & 0xFFEFFFFF) | ((arg2 & 1) << 0x14);
        return;
    case 0x5A:                                      /* switch 1 */
        temp_a0->script_value98 = arg2;
        return;
    case 0x5B:                                      /* switch 1 */
        temp_a0->script_value9a = arg2;
        return;
    case 0x5C:                                      /* switch 1 */
        temp_a0->script_value9e = arg2;
        return;
    case 0x5D:                                      /* switch 1 */
        temp_a0->script_value9f = arg2;
        return;
    case 0x5E:                                      /* switch 1 */
        temp_a0->script_valueae = arg2;
        return;
    case 0x5F:                                      /* switch 1 */
        temp_a0->script_valueaf = arg2;
        return;
    case 0x60:                                      /* switch 1 */
        temp_a0->script_valuebc = arg2;
        return;
    case 0x32:                                      /* switch 1 */
        temp_a0->script_values_b0[0] = arg2;
        return;
    case 0x33:                                      /* switch 1 */
        temp_a0->script_values_b0[1] = arg2;
        return;
    case 0x34:                                      /* switch 1 */
        temp_a0->script_values_b0[2] = arg2;
        return;
    case 0x35:                                      /* switch 1 */
        temp_a0->script_values_b0[3] = arg2;
        return;
    case 0x36:                                      /* switch 1 */
        temp_a0->script_values_b0[4] = arg2;
        return;
    case 0x37:                                      /* switch 1 */
        temp_a0->script_values_b0[5] = arg2;
        return;
    case 0x64:
        temp_a0->status_flags2.bits.field64 = arg2;
        return;
    case 0x65:
        temp_a0->status_flags2.bits.field65 = arg2;
        return;
    case 0x66:
        temp_a0->status_flags2.bits.field66 = arg2;
        return;
    case 0x67:
        temp_a0->status_flags2.bits.field67 = arg2;
        return;
    case 0x68:
        temp_a0->status_flags2.bits.field68 = arg2;
        return;
    case 0x69:
        temp_a0->status_flags2.bits.field69 = arg2;
        return;
    case 0x6A:
        temp_a0->status_flags2.bits.field6a = arg2;
        return;
    case 0x6B:
        temp_a0->status_flags2.bits.field6b = arg2;
        return;
    case 0x6C:
        temp_a0->status_flags2.bits.field6c = arg2;
        return;
    case 0x6D:
        temp_a0->status_flags2.bits.field6d = arg2;
        return;
    case 0x6E:
        temp_a0->status_flags2.bits.field6e = arg2;
        return;
    case 0x78:                                      /* switch 1 */
        temp_a0->panel_c_value = arg2;
        return;
    case 0x79:                                      /* switch 1 */
        temp_a0->panel_c_timer = arg2;
        return;
    case 0x7A:                                      /* switch 1 */
        temp_a0->panel_c_mode = arg2;
        return;
    case 0x7B:                                      /* switch 1 */
        temp_a0->loot_item_id = arg2;
        return;
    case 0x7C:                                      /* switch 1 */
        temp_a0->loot_item_aux = arg2;
        /* fallthrough */
    default:                                        /* switch 1 */
        return;
    }
}
