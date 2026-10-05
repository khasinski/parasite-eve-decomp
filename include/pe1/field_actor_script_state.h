#ifndef PE1_FIELD_ACTOR_SCRIPT_STATE_H
#define PE1_FIELD_ACTOR_SCRIPT_STATE_H

#include "common.h"

typedef struct FieldActorScriptState {
    u32 core_flags;
    u8 reserved_04[0x14];
    u8 *substate;
} FieldActorScriptState;

PE1_STATIC_ASSERT(PE1_OFFSETOF(FieldActorScriptState, substate) == 0x18,
                  field_actor_script_state_substate_offset);
PE1_STATIC_ASSERT(sizeof(FieldActorScriptState) == 0x1C,
                  field_actor_script_state_size);

#endif
