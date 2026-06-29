/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

typedef struct QueueEntry {
    struct QueueEntry *next;
    int value0;
    int value1;
} QueueEntry;

QueueEntry *g_MenuEventQueueFreeList;
QueueEntry *g_MenuEventQueueHead;
QueueEntry *g_MenuEventQueueTail;

void BoundsCheck_AssertStub(int arg0);

void Queue_Enqueue(int arg0, int arg1) {
    QueueEntry *entry;
    QueueEntry *tail;

    entry = g_MenuEventQueueFreeList;
    if (entry != 0) {
        g_MenuEventQueueFreeList = entry->next;
        entry->next = 0;
        tail = g_MenuEventQueueTail;
        if (tail != 0) {
            tail->next = entry;
        } else {
            if (g_MenuEventQueueHead != 0) {
                BoundsCheck_AssertStub(0x1F);
            }
            g_MenuEventQueueHead = entry;
        }
        g_MenuEventQueueTail = entry;
        entry->value0 = arg0;
        entry->value1 = arg1;
    }
}
