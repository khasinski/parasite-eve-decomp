/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

typedef struct QueueEntry {
    struct QueueEntry *next;
    int value0;
    int value1;
} QueueEntry;

volatile QueueEntry *g_MenuEventQueueFreeList;
QueueEntry *g_MenuEventQueueHead;
QueueEntry *g_MenuEventQueueTail;

void Queue_DequeueByMask(int mask, QueueEntry *out_arg) {
    QueueEntry *out;
    QueueEntry *entry;
    QueueEntry *prev;
    QueueEntry *head;

    out = out_arg;
    if (out == 0) {
        return;
    }

    head = g_MenuEventQueueHead;
    if (head == 0) {
        goto fail;
    }

    entry = head;
    prev = 0;
    if ((entry->value0 & mask) == 0) {
        do {
            prev = entry;
            entry = entry->next;
            if (entry == 0) {
                goto fail;
            }
        } while ((entry->value0 & mask) == 0);
    }

    if (entry == 0) {
        goto fail;
    }

    if (prev != 0) {
        prev->next = entry->next;
    } else {
        g_MenuEventQueueHead = entry->next;
    }

    if (entry == g_MenuEventQueueTail) {
        g_MenuEventQueueTail = prev;
    }

    {
        QueueEntry *oldFree = (QueueEntry *)g_MenuEventQueueFreeList;
        g_MenuEventQueueFreeList = entry;
        entry->next = oldFree;
    }
    *out = *entry;
    return;

fail:
    out->value0 = 0;
    out->value1 = 0;
}
