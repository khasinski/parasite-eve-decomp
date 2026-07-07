
typedef struct Entity Entity;

struct Entity {
    char pad0[0x98];
    int flags98;
};

extern Entity *g_CurrentEntity[];

int Entity_TestCurrentFlags(int **arg0) {
    Entity *entity;
    int flags;
    int mask;

    entity = g_CurrentEntity[0];
    flags = entity->flags98;
    mask = *arg0[0];
    *arg0[1] = (flags & mask) == mask;
    return 1;
}
