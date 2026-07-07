
typedef unsigned char u8;
typedef unsigned short u16;

typedef struct Entity Entity;

struct Entity {
    char pad0[0xF];
    u8 fieldF;
    char pad10[4];
    int field14;
};

extern Entity *g_CurrentEntity[];

int Task_SetEntityAnimSpeed(u16 **arg0) {
    Entity *entity;
    unsigned int value;

    entity = g_CurrentEntity[0];
    value = **arg0;
    if (entity->fieldF < value) {
        value = entity->fieldF;
    }
    entity->field14 = value << 16;
    return 1;
}
