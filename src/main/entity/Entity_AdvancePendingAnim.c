/* CC1_PROFILE: build0001 */
/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

typedef unsigned short u16;

typedef struct Entity Entity;
typedef struct Node Node;

struct Node {
    int current;
    int next_value;
    u16 flags;
    char padA[6];
    int active;
    char pad14[0x10];
    Node *next;
};

struct Entity {
    char pad0[0xA0];
    Node *lists[3];
};

extern Entity *g_CurrentEntity[];

int Entity_AdvancePendingAnim(void) {
    unsigned int i;
    Entity *entity;
    Node *node;
    int active;

    i = 0;
    active = 1;
    entity = g_CurrentEntity[0];
    do {
        node = entity->lists[0];
        if (node != 0) {
            do {
                if (node->flags & 0x40) {
                    node->flags &= ~0x40;
                    if (node->next_value != 0) {
                        node->current = node->next_value;
                        node->active = active;
                        node->flags &= ~0x20;
                    }
                }
                node = node->next;
            } while (node != 0);
        }
        i++;
        entity = (Entity *)((char *)entity + 4);
    } while (i < 3U);
    return 1;
}
