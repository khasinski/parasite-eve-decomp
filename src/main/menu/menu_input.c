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
extern int D_8009D0E8;
extern int D_8009D0F0;
extern int D_8009D0F4;
extern int D_8009D0F8;

int Draw_RemapStatusFlags(int flags);
void BoundsCheck_AssertStub(int arg0);

void MenuInput_EnqueueStatusChanges(int flags) {
    int flags_reg;
    MenuInputQueuedEvent *event;
    MenuInputQueuedEvent *tail;
    int mapped;
    int released;
    int timer;
    register int prev_flags asm("$3");
    int repeat_reset;
    int repeat_step;
    register int type asm("$2");
    register int release_type asm("$20");

    flags_reg = flags;
    repeat_reset = 0;
    mapped = Draw_RemapStatusFlags(flags_reg);
    if (D_8009D0E8 != 0) {
        register int inverse asm("$2");
        prev_flags = D_8009D0F0;
        inverse = ~mapped;
        released = inverse & prev_flags;
        if (released != 0) {
            event = D_8009D0DC;
            release_type = 4;
            if (event != 0) {
                {
                    MenuInputQueuedEvent *next = event->next;
                    asm("" : : "r"(next) : "memory");
                    D_8009D0DC = next;
                }
                event->next = 0;
                tail = D_8009D0E4;
                if (tail != 0) {
                    tail->next = event;
                } else {
                    if (D_8009D0E0 != 0) {
                        BoundsCheck_AssertStub(0x1F);
                    }
                    D_8009D0E0 = event;
                }
                D_8009D0E4 = event;
                event->type = release_type;
                event->flags = released;
            }
        }

        if (D_8009D0F0 != mapped) {
            D_8009D0F8 = 0x10;
        }

        timer = D_8009D0F8 - 2;
        D_8009D0F8 = timer;
        if (timer < 0) {
            if (flags_reg != 0 && timer < -0x5A) {
                goto reset_repeat;
            }
            if ((timer & 3) == 0) {
reset_repeat:
                D_8009D0F0 = 0;
                repeat_reset = 1;
            }
        }

        if (D_8009D0F8 >= -0x12B) {
            repeat_step = 1;
        } else {
            repeat_step = 8;
        }
        D_8009D0F4 = repeat_step;

        {
            register int previous asm("$2");
            previous = D_8009D0F0;
            released = mapped & ~previous;
        }
        if (released != 0) {
            if (repeat_reset != 0 && (released & 0x40) != 0) {
                goto done;
            }
            type = repeat_reset != 0 ? 2 : 1;
            event = D_8009D0DC;
            if (event != 0) {
                register MenuInputQueuedEvent *next asm("$2");
                register int event_type asm("$19");
                event_type = type;
                next = event->next;
                tail = D_8009D0E4;
                event->next = 0;
                D_8009D0DC = next;
                if (tail != 0) {
                    tail->next = event;
                } else {
                    if (D_8009D0E0 != 0) {
                        BoundsCheck_AssertStub(0x1F);
                    }
                    D_8009D0E0 = event;
                }
                D_8009D0E4 = event;
                event->type = event_type;
                event->flags = released;
            }
        }
done:
        D_8009D0F0 = mapped;
    } else if (mapped == 0) {
        D_8009D0E8 = 1;
    }
}

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
