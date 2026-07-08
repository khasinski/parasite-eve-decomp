#ifndef PE1_MENU_QUEUE_H
#define PE1_MENU_QUEUE_H

typedef struct MenuQueueEntry {
    struct MenuQueueEntry *next;
    int value0;
    int value1;
} MenuQueueEntry;

extern MenuQueueEntry D_800A2090[];
extern MenuQueueEntry D_800A2174[];
extern MenuQueueEntry *g_MenuEventQueueHead;
extern MenuQueueEntry *g_MenuEventQueueTail;

#endif /* PE1_MENU_QUEUE_H */
