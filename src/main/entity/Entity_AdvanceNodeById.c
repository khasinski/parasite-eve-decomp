/* CC1_FLAGS: -g3 */

typedef unsigned short u16;

typedef struct Entity Entity;
typedef struct Node Node;

struct Node {
    int current;
    int next_value;
    u16 flags;
    u16 id;
    char padC[4];
    int active;
    char pad14[0x10];
    Node *next;
};

struct Entity {
    char pad0[0xA0];
    Node *lists[3];
};

extern Entity *g_CurrentEntity[];

int Entity_AdvanceNodeById(int **arg0) {
    unsigned int i;
    Entity *entity;
    Node *node;
    int next_value;
    int active_value;

    i = 0;
    active_value = 1;
    entity = g_CurrentEntity[0];
    do {
        node = entity->lists[0];
        if (node != 0) {
            do {
                if (node->id == **arg0) {
                    node->flags &= ~0x40;
                    next_value = node->next_value;
                    if (next_value != 0) {
                        node->current = next_value;
                        node->active = active_value;
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
