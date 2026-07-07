/* CC1_FLAGS: -G0 -g3 */
/* MASPSX_FLAGS: -G0 */

typedef unsigned char u8;
typedef unsigned int u32;

typedef struct Entity Entity;

struct Entity {
    char pad0[0x4];
    Entity *next;
    char pad8[0xC - 0x8];
    u8 field0C;
    u8 pad0D;
    u8 field0E;
    u8 field0F;
    char pad10[0x14 - 0x10];
    int frame14;
    int frame18;
    char pad1C[0x98 - 0x1C];
    u32 flags98;
    char pad9C[0x18C - 0x9C];
    Entity *parent18C;
    char pad190[0x1B0 - 0x190];
    void *action1B0;
};

extern Entity *g_FieldActorListHead;
extern void *D_800B0E98;

void Entity_SetActionMode(Entity *arg0, int arg1) {
    Entity *entity;
    int mode;
    unsigned int idx;
    void *base;
    Entity *it;
    void **table;
    void *action;

    entity = arg0;
    mode = arg1;
    idx = entity->field0C;
    base = &D_800B0E98;
    table = (void **)((char *)base + ((idx * 3) << 6));
    action = table[(unsigned short)mode];
    entity->field0E = mode;
    entity->frame14 = 0;
    entity->frame18 = 0;
    entity->action1B0 = action;
    entity->flags98 &= -0x201;
    entity->field0F = *((u8 *)entity->action1B0 + 2) - 1;

    if (entity->flags98 & 0x100000) {
        it = g_FieldActorListHead;
        if (it != 0) {
            do {
                if (it->parent18C == entity && (it->flags98 & 0x200000)) {
                    Entity_SetActionMode(it, (unsigned short)mode);
                }
                it = it->next;
            } while (it != 0);
        }
    }
}

void Entity_SetAction(Entity *arg0, int arg1) {
    Entity *entity;
    int raw_mode;
    unsigned int mode;
    unsigned int idx;
    void *base;
    Entity *it;
    void **table;
    void *action;

    entity = arg0;
    raw_mode = arg1;
    mode = (unsigned short)raw_mode;
    if (entity->field0E != mode) {
        idx = entity->field0C;
        base = &D_800B0E98;
        table = (void **)((char *)base + ((idx * 3) << 6));
        entity->frame14 = 0;
        entity->frame18 = 0;
        entity->field0E = raw_mode;
        action = table[mode];
        entity->action1B0 = action;
        entity->field0F = *((u8 *)action + 2) - 1;
    }

    entity->flags98 &= -0x201;
    if (entity->flags98 & 0x100000) {
        it = g_FieldActorListHead;
        if (it != 0) {
            do {
                if (it->parent18C == entity && (it->flags98 & 0x200000)) {
                    Entity_SetAction(it, (unsigned short)raw_mode);
                }
                it = it->next;
            } while (it != 0);
        }
    }
}
