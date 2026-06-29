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

void Entity_TickAnimSequences(Entity *arg0) {
    unsigned int i;
    Node *node;

    i = 0;
    do {
        node = arg0->lists[0];
        if (node != 0) {
            do {
                if (node->next_value != 0) {
                    node->current = node->next_value;
                    node->active = 1;
                    node->flags &= ~0x60;
                }
                node = node->next;
            } while (node != 0);
        }
        i++;
        arg0 = (Entity *)((char *)arg0 + 4);
    } while (i < 3U);
}
