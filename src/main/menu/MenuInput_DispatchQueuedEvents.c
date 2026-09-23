/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: --use-comm-section -G8 */


#include "pe1/psyq_nop.h"

typedef struct MenuInputQueuedEvent {
    struct MenuInputQueuedEvent *next;
    int type;
    int flags;
} MenuInputQueuedEvent;

typedef struct MenuInputWidget {
    char unk_00[4];
    struct MenuInputWidget *next;
    char unk_08[0x20];
    int input_flags;
    int (*handler)(struct MenuInputWidget *node, int flags);
} MenuInputWidget;

MenuInputWidget *MenuWidget_GetCurrentNode(void);

extern int D_8009D0EC;
extern MenuInputQueuedEvent *D_8009D0E0;
extern MenuInputQueuedEvent *D_8009D0E4;
extern MenuInputQueuedEvent * volatile D_8009D0DC;


void MenuInput_EnqueueStatusChanges(int flags);
void MenuInput_DispatchQueuedEvents(void) {
    MenuInputWidget *node;
    MenuInputQueuedEvent *event;
    MenuInputQueuedEvent *prev;
    register MenuInputQueuedEvent *head asm("$2");
    MenuInputQueuedEvent *free_head;
    MenuInputQueuedEvent local;
    MenuInputQueuedEvent *localp;
    int type;
    int flags;
    int handled;

    node = MenuWidget_GetCurrentNode();
    if (D_8009D0EC == 0) {
        MenuInput_EnqueueStatusChanges(node->input_flags);
    }

    localp = &local;
    head = D_8009D0E0;
    if (head != 0) {
        event = head;
        prev = 0;
        while (event != 0 && event->type == 0) {
            prev = event;
            event = event->next;
        }

        if (event != 0) {
            if (prev != 0) {
                prev->next = event->next;
            } else {
                MenuInputQueuedEvent *next = event->next;
                PE1_NOP_DEP("r", next);
                D_8009D0E0 = next;
            }
            if (event == D_8009D0E4) {
                D_8009D0E4 = prev;
            }
            free_head = D_8009D0DC;
            D_8009D0DC = event;
            event->next = free_head;
            *localp = *event;
        } else {
            localp->type = 0;
            localp->flags = 0;
        }
    } else {
        asm("" : "=r"(head) : "0"(head));
        local.type = 0;
        local.flags = 0;
    }

    type = local.type;
    if (type <= 0) {
        return;
    }

    if (type >= 3 && type != 4) {
        return;
    }
    if (type < 3) {
        if (node == 0) {
            return;
        }
        do {
            flags = local.flags;
            if (local.type == 2) {
                flags |= 0x20000;
            }
            handled = node->handler(node, flags);
            if (handled != 0) {
                return;
            }
            node = node->next;
        } while (node != 0);
    } else {
        if ((local.flags & 0x20) == 0) {
            return;
        }
        node = MenuWidget_GetCurrentNode();
        if (node == 0) {
            return;
        }
        do {
            handled = node->handler(node, 0x10000);
            if (handled != 0) {
                return;
            }
            node = node->next;
        } while (node != 0);
    }
}
