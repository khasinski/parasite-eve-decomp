/* CC1_FLAGS: -g3 */

typedef struct Entity Entity;

struct Entity {
    char pad0[0x224];
    short field224;
};

extern Entity *g_CurrentEntity[];

int Task_SetEntityField224(short **arg0) {
    Entity *entity;
    int value;

    entity = g_CurrentEntity[0];
    value = (*arg0)[1];
    entity->field224 = value;
    return 1;
}
