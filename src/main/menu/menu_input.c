/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: --use-comm-section -G8 */


#include "include_asm.h"
int g_MenuInputPollingPaused;

int g_MenuDrawPhase;

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

/* Function body emitted as asm() (PSYQ GCC 2.7.2 register
 * allocation / scheduling diverges from the ROM pervasively).
 * Wrapped via maspsx's INCLUDE_ASM hack so the body is passed
 * through verbatim; bare global loads gp-rel via the .extern size
 * hints + `as -G8`. See tools/maspsx/PE1_IMPROVEMENTS.md. */
void __maspsx_include_asm_hack_MenuInput_EnqueueStatusChanges(void);

/* Function body emitted as asm() (PSYQ GCC 2.7.2 register
 * allocation / scheduling diverges from the ROM pervasively).
 * Wrapped via maspsx's INCLUDE_ASM hack so the body is passed
 * through verbatim; bare global loads gp-rel via the .extern size
 * hints + `as -G8`. See tools/maspsx/PE1_IMPROVEMENTS.md. */
void __maspsx_include_asm_hack_MenuInput_DispatchQueuedEvents(void);

void MenuInput_SetPollingPaused(int arg0) {
    g_MenuInputPollingPaused = arg0;
}

int MenuInput_GetRepeatStep(void) {
    return g_MenuDrawPhase;
}

INCLUDE_ASM("asm/USA/main/nonmatchings/menu/menu_input", MenuInput_EnqueueStatusChanges);

extern int D_8009D0EC;
extern MenuInputQueuedEvent *D_8009D0E0;
extern MenuInputQueuedEvent *D_8009D0E4;
extern MenuInputQueuedEvent * volatile D_8009D0DC;

void MenuInput_DispatchQueuedEvents(void) {
    MenuInputWidget *node;
    register MenuInputQueuedEvent *event asm("$6");
    register MenuInputQueuedEvent *prev asm("$3");
    register MenuInputQueuedEvent *head asm("$2");
    register MenuInputQueuedEvent *free_head asm("$2");
    MenuInputQueuedEvent local;
    register MenuInputQueuedEvent *localp asm("$7");
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
                D_8009D0E0 = event->next;
            }
            if (D_8009D0E4 == event) {
                D_8009D0E4 = prev;
            }
            free_head = D_8009D0DC;
            D_8009D0DC = event;
            event->next = free_head;
            localp->next = *(MenuInputQueuedEvent * volatile *)&event->next;
            localp->type = event->type;
            localp->flags = event->flags;
        } else {
            localp->type = 0;
            localp->flags = 0;
        }
    } else {
        localp->type = 0;
        localp->flags = 0;
    }

    type = localp->type;
    if (type <= 0) {
        return;
    }

    if (type < 3) {
        if (node == 0) {
            return;
        }
        do {
            flags = localp->flags;
            if (localp->type == 2) {
                flags |= 0x20000;
            }
            handled = node->handler(node, flags);
            if (handled != 0) {
                return;
            }
            node = node->next;
        } while (node != 0);
    } else if (type == 4) {
        if ((localp->flags & 0x20) == 0) {
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
