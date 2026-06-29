/* CC1_FLAGS: -G0 -g3 */
/* MASPSX_FLAGS: -G0 --use-comm-section */

typedef unsigned int u32;

typedef struct Entity Entity;

struct Entity {
    char pad0[0x4];
    Entity *next;
    char pad8[0x98 - 0x8];
    u32 flags98;
    char pad9C[0x18C - 0x9C];
    Entity *parent18C;
};

extern Entity *g_FieldActorListHead;
extern Entity *g_CurrentEntity;

int Task_ClearEntityParentLink(void) {
    Entity *current;
    Entity *it;

    current = g_CurrentEntity;
    it = g_FieldActorListHead;
    current->parent18C = 0;
    current->flags98 &= 0xFF9FFFFF;

    if (it != 0) {
        do {
            if (it != current) {
                if (it->parent18C == current->parent18C) {
                    return 1;
                }
            }
            it = it->next;
        } while (it != 0);
    }

    {
        Entity *tail_current;
        int clear_mask;
        tail_current = g_CurrentEntity;
        clear_mask = 0xFFEFFFFF;
        tail_current->parent18C->flags98 &= clear_mask;
    }
    return 1;
}
