#ifndef PE1_FIELD_ANIM_CALLBACK_LIST_H
#define PE1_FIELD_ANIM_CALLBACK_LIST_H

typedef int (*FieldAnimCallbackListCallback)(int mode, void *state);

typedef struct FieldAnimCallbackListEntry {
    signed short active;
    signed short age;
    unsigned char state[1];
} FieldAnimCallbackListEntry;

typedef struct FieldAnimCallbackList {
    int stride;
    int count;
    FieldAnimCallbackListCallback callback;
    unsigned char entries[1];
} FieldAnimCallbackList;

#endif /* PE1_FIELD_ANIM_CALLBACK_LIST_H */
