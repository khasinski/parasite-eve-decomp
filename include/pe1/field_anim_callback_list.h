#ifndef PE1_FIELD_ANIM_CALLBACK_LIST_H
#define PE1_FIELD_ANIM_CALLBACK_LIST_H

typedef int (*FieldAnimCallbackListCallback)(int mode, void *state, int argument);

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

/* Effect task pools: func_800CE560 lays out a list of count entries of
 * stride bytes (plus the active/age prefix) behind its header and returns
 * the bytes used; func_800CE610 claims the first inactive entry and returns
 * its state, or null when the list is full. */
int func_800CE560(char *out, int stride, int count,
                  FieldAnimCallbackListCallback callback);
void *func_800CE610(char *list);

#endif /* PE1_FIELD_ANIM_CALLBACK_LIST_H */
