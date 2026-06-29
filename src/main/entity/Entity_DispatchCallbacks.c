/* CC1_PROFILE: build0001 */
/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

typedef struct Entity Entity;

struct Entity {
    char pad0[0xA0];
    int callbacks[3];
};

Entity *g_CurrentEntity;
extern int g_TaskNodePool[];

void Task_RunQueue(void);

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
