/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 */

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
extern Node *g_TaskNodePool;

int Entity_MarkAnimPending(void) {
    unsigned int i;
    Entity *entity;
    Node *node;
    Node *skip;

    i = 0;
    skip = g_TaskNodePool;
    entity = g_CurrentEntity[0];
    do {
        node = entity->lists[0];
        if (node != 0) {
            do {
                if (node != skip) {
                    node->flags |= 0x40;
                }
                node = node->next;
            } while (node != 0);
        }
        i++;
        entity = (Entity *)((char *)entity + 4);
    } while (i < 3U);
    return 1;
}
