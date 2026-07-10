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
    register int flags_reg asm("$21");
    register MenuInputQueuedEvent *event asm("$16");
    MenuInputQueuedEvent *tail;
    register int mapped asm("$18");
    register int released asm("$17");
    int timer;
    int prev_flags;
    register int repeat_reset asm("$19");
    int repeat_step;
    int type;
    register int release_type asm("$20");

    flags_reg = flags;
    repeat_reset = 0;
    mapped = Draw_RemapStatusFlags(flags_reg);
    if (D_8009D0E8 != 0) {
        prev_flags = D_8009D0F0;
        released = ~mapped & prev_flags;
        if (released != 0) {
            event = D_8009D0DC;
            release_type = 4;
            if (event != 0) {
                D_8009D0DC = event->next;
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

        released = mapped & ~D_8009D0F0;
        if (released != 0) {
            type = 1;
            if (repeat_reset != 0) {
                if ((released & 0x40) != 0) {
                    goto done;
                }
                type = 2;
            }
            if (D_8009D0DC != 0) {
                event = D_8009D0DC;
                D_8009D0DC = event->next;
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
                event->type = type;
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
