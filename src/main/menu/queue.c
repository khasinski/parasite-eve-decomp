/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: --use-comm-section -G8 */

#include "pe1/menu_queue.h"

MenuQueueEntry *g_MenuEventQueueFreeList;
MenuQueueEntry *g_MenuEventQueueHead;
MenuQueueEntry *g_MenuEventQueueTail;
int g_MenuInputActive;
int g_MenuInputPollingPaused;
int g_MenuInputHeldStatusMask;


#include "pe1/bounds_check.h"

void Queue_Init(void) {
    MenuQueueEntry *entry;
    MenuQueueEntry *end;

    entry = D_800A2090;
    end = D_800A2090 + 20;
    if (entry < end) {
        do {
            entry->next = entry + 1;
            entry = entry->next;
        } while (entry < end);
    }

    D_800A2174[0].next = 0;
    g_MenuEventQueueFreeList = (MenuQueueEntry *)((char *)D_800A2174 - 0xE4);
    g_MenuEventQueueTail = 0;
    g_MenuEventQueueHead = 0;
    g_MenuInputActive = 0;
    g_MenuInputPollingPaused = 0;
    g_MenuInputHeldStatusMask = 0;
}

void Queue_Enqueue(int arg0, int arg1) {
    MenuQueueEntry *entry;
    MenuQueueEntry *tail;

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
        entry->payload.values.value0 = arg0;
        entry->payload.values.value1 = arg1;
    }
}

void Queue_DequeueByMask(int mask, MenuQueueEntry *out_arg) {
    MenuQueueEntry *out;
    MenuQueueEntry *entry;
    MenuQueueEntry *prev;
    MenuQueueEntry *head;

    out = out_arg;
    if (out == 0) {
        return;
    }

    head = g_MenuEventQueueHead;
    if (head != 0) {
        entry = head;
        prev = 0;
        while (entry != 0 && (entry->payload.values.value0 & mask) == 0) {
            prev = entry;
            entry = entry->next;
        }

        if (entry != 0) {
            if (prev != 0) {
                prev->next = entry->next;
            } else {
                g_MenuEventQueueHead = entry->next;
            }

            if (entry == g_MenuEventQueueTail) {
                g_MenuEventQueueTail = prev;
            }

            {
                MenuQueueEntry *oldFree = (MenuQueueEntry *)g_MenuEventQueueFreeList;
                g_MenuEventQueueFreeList = entry;
                entry->next = oldFree;
            }
            *out = *entry;
            return;
        }
    }

    out->payload.values.value0 = 0;
    out->payload.values.value1 = 0;
}
