/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: --use-comm-section -G8 */

#include "pe1/menu_queue.h"

MenuQueueEntry *g_MenuEventQueueFreeList;
MenuQueueEntry *g_MenuEventQueueHead;
MenuQueueEntry *g_MenuEventQueueTail;
int g_MenuInputActive;
int g_MenuInputPollingPaused;
int g_MenuInputHeldStatusMask;

MenuQueueEntry *g_MenuEventQueueFreeList;
MenuQueueEntry *g_MenuEventQueueHead;
MenuQueueEntry *g_MenuEventQueueTail;

void BoundsCheck_AssertStub(int arg0);

void Queue_Init(void) {
    MenuQueueEntry *entry;
    MenuQueueEntry *end;

    entry = D_800A2090;
    end = (MenuQueueEntry *)((char *)D_800A2090 + 0xF0);
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
        entry->value0 = arg0;
        entry->value1 = arg1;
    }
}
