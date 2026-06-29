/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

typedef struct QueueEntry {
    struct QueueEntry *next;
    int value0;
    int value1;
} QueueEntry;

extern QueueEntry D_800A2090[];
extern QueueEntry D_800A2174[];
QueueEntry *g_MenuEventQueueFreeList;
QueueEntry *g_MenuEventQueueHead;
QueueEntry *g_MenuEventQueueTail;
int g_MenuInputActive;
int g_MenuInputPollingPaused;
int g_MenuInputHeldStatusMask;

void Queue_Init(void) {
    QueueEntry *entry;
    QueueEntry *end;

    entry = D_800A2090;
    end = (QueueEntry *)((char *)D_800A2090 + 0xF0);
    if (entry < end) {
        do {
            entry->next = entry + 1;
            entry = entry->next;
        } while (entry < end);
    }

    D_800A2174[0].next = 0;
    g_MenuEventQueueFreeList = (QueueEntry *)((char *)D_800A2174 - 0xE4);
    g_MenuEventQueueTail = 0;
    g_MenuEventQueueHead = 0;
    g_MenuInputActive = 0;
    g_MenuInputPollingPaused = 0;
    g_MenuInputHeldStatusMask = 0;
}
