#ifndef PE1_PLAYER_ENTITY_H
#define PE1_PLAYER_ENTITY_H

#include "pe1/field_actor.h"

/* Active player actor (0x8009D254). Units whose retail code reaches it with
 * an absolute address (array views) or through battle types still declare
 * their own view; see the note in field_actor.h. */
extern FieldActor *g_PlayerEntity;

#endif /* PE1_PLAYER_ENTITY_H */
