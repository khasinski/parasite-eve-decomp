/* CC1_FLAGS: -g3 */

typedef unsigned short u16;

typedef struct Entity Entity;
typedef struct Node Node;

struct Node {
    int current;
    int next_value;
    u16 flags;
    u16 id;
    char padC[0x18];
    Node *next;
};

struct Entity {
    char pad0[0xA0];
    Node *lists[3];
};

extern Entity *g_CurrentEntity[];

int Entity_MarkNodeAnimPendingById(int **arg0) {
    unsigned int i;
    Entity *entity;
    Node *node;

    i = 0;
    entity = g_CurrentEntity[0];
    do {
        node = entity->lists[0];
        if (node != 0) {
            do {
                if (node->id == **arg0) {
                    node->flags |= 0x40;
                    return 1;
                }
                node = node->next;
            } while (node != 0);
        }
        i++;
        entity = (Entity *)((char *)entity + 4);
    } while (i < 3U);

    return 1;
}
