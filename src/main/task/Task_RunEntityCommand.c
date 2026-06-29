/* CC1_PROFILE: build0001 */
/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

typedef struct Entity Entity;

extern Entity *g_CurrentEntity[];
extern int g_SceneDataTable0;
extern char *g_TaskNodePool;

int Entity_TriggerAnimEvent(Entity *arg0, unsigned char arg1);

int Task_RunEntityCommand(int **arg0) {
    int result;
    register int active asm("$5");
    register int stack asm("$4");
    char *state;

    result = (signed char)Entity_TriggerAnimEvent(g_CurrentEntity[0], *(unsigned char *)arg0[0]);
    *arg0[1] = result;
    if (*arg0[1] != 0) {
        return 1;
    }

    active = 1;
    stack = g_SceneDataTable0;
    state = g_TaskNodePool;
    stack -= 0x10;
    *(int *)(state + 0x10) = active;
    g_SceneDataTable0 = stack;
    return 0;
}
