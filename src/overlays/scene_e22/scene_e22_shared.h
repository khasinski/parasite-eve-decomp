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

typedef struct SceneE22RangeCallbackState {
    char pad00[8];
    u8 *data;
    void (*callback)(void *owner);
    char pad10[6];
    s8 state_id;
    s8 range_value;
} SceneE22RangeCallbackState;

PE1_STATIC_ASSERT(PE1_OFFSETOF(SceneE22RangeCallbackState, data) == 8,
                  scene_e22_range_callback_data_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(SceneE22RangeCallbackState, callback) == 0xC,
                  scene_e22_range_callback_callback_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(SceneE22RangeCallbackState, state_id) == 0x16,
                  scene_e22_range_callback_state_id_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(SceneE22RangeCallbackState, range_value) == 0x17,
                  scene_e22_range_callback_range_value_offset);
PE1_STATIC_ASSERT(sizeof(SceneE22RangeCallbackState) == 0x18,
                  scene_e22_range_callback_state_size);

#endif /* SCENE_E22_SHARED_H */
