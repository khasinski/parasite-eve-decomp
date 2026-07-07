/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: --use-comm-section -G8 */

typedef struct Entity Entity;

struct Entity {
    char pad0[0xA0];
    int callbacks[3];
};

Entity *g_CurrentEntity;
extern int g_TaskNodePool[];

void Task_RunQueue(void);

typedef unsigned short u16;

typedef struct Entity_1 Entity_1;
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

struct Entity_1 {
    char pad0[0xA0];
    Node *lists[3];
};

void Entity_DispatchCallbacks(Entity *arg0) {
    int i;
    int *ptr;
    int callback;

    i = 0;
    ptr = arg0->callbacks;
    g_CurrentEntity = arg0;
    do {
        callback = *ptr;
        g_TaskNodePool[0] = callback;
        if (callback != 0) {
            Task_RunQueue();
        }
        i++;
        ptr++;
    } while ((unsigned int)i < 3U);
}

void Entity_TickAnimSequences(Entity_1 *arg0) {
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
        arg0 = (Entity_1 *)((char *)arg0 + 4);
    } while (i < 3U);
}
