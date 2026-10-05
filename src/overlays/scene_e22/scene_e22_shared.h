#ifndef SCENE_E22_SHARED_H
#define SCENE_E22_SHARED_H

#include "common.h"

typedef struct SceneE22CallbackRecord {
    char pad00[0xC];
    void (*callback)(void *owner);
} SceneE22CallbackRecord;

PE1_STATIC_ASSERT(PE1_OFFSETOF(SceneE22CallbackRecord, callback) == 0xC,
                  scene_e22_callback_record_callback_offset);
PE1_STATIC_ASSERT(sizeof(SceneE22CallbackRecord) == 0x10,
                  scene_e22_callback_record_size);

#endif /* SCENE_E22_SHARED_H */
