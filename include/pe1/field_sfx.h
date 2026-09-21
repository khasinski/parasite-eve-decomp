#ifndef PE1_FIELD_SFX_H
#define PE1_FIELD_SFX_H

#include "pe1/field_actor.h"

typedef unsigned char FieldSfxU8;
typedef unsigned short FieldSfxU16;

typedef struct FieldSfxQueueEntry {
    FieldSfxU16 typeId;
    FieldSfxU8 subId;
    FieldSfxU8 taskArgument;
    /* Enqueued as a word; dispatch reads only its first (low) byte. */
    union { int word; FieldSfxU8 id; } actorSelector;
    int taskValue;
} FieldSfxQueueEntry;

PE1_STATIC_ASSERT(sizeof(FieldSfxQueueEntry) == 12, field_sfx_entry_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(FieldSfxQueueEntry, actorSelector) == 4,
                  field_sfx_selector_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(FieldSfxQueueEntry, taskValue) == 8,
                  field_sfx_value_offset);

extern FieldActor *g_FieldActorListHead;
void Scene_UpdateEntityList(void);

extern FieldSfxU8 g_FieldSfxQueueCount;
extern FieldSfxQueueEntry g_FieldSfxQueue[28];

#endif /* PE1_FIELD_SFX_H */
