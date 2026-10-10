#ifndef PE1_MENU_QUEUE_H
#define PE1_MENU_QUEUE_H

#include "common.h"

void Queue_Init(void);

typedef struct MenuQueueEntry {
    struct MenuQueueEntry *next;
    union {
        struct {
            int value0;
            int value1;
        } values;
        struct {
            int type;
            int flags;
        } input;
    } payload;
} MenuQueueEntry;

typedef MenuQueueEntry MenuInputQueuedEvent;

void Queue_Enqueue(int value0, int value1);
void Queue_DequeueByMask(int mask, MenuQueueEntry *out);

PE1_STATIC_ASSERT(sizeof(MenuQueueEntry) == 0x0C, menu_queue_entry_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(MenuQueueEntry, next) == 0x00,
                  menu_queue_entry_next_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(MenuQueueEntry, payload) == 0x04,
                  menu_queue_entry_payload_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(MenuQueueEntry, payload.input.flags) == 0x08,
                  menu_queue_input_flags_offset);

extern MenuQueueEntry D_800A2090[];
extern MenuQueueEntry D_800A2174[];
extern MenuQueueEntry *g_MenuEventQueueFreeList;
extern int g_MenuInputActive;
extern int g_MenuInputPollingPaused;
extern int g_MenuInputHeldStatusMask;
extern MenuQueueEntry *g_MenuEventQueueHead;
extern MenuQueueEntry *g_MenuEventQueueTail;


void MenuInput_DispatchQueuedEvents(void);
void Draw_SelectBuffer(void);
void Draw_PresentFrame(int mode);

#endif /* PE1_MENU_QUEUE_H */
