#ifndef ROOMLIB_RESET_ACTOR_POSITION_NAME
#define ROOMLIB_RESET_ACTOR_POSITION_NAME func_8018F6C4
#endif

typedef struct RoomResetActorChild {
    char pad0[0xC];
    short active;
} RoomResetActorChild;

typedef struct RoomResetActor {
    RoomResetActorChild *child;
    char pad4[0x24];
    int pos[3];
    char pad34[0xC];
    int basePos[3];
    char pad4C[0x1C];
    int velocity[3];
    char pad74[0x4];
    int movement[3];
    char pad84[0x14];
    unsigned int flags;
    char pad9C[0x13C];
    int field1D8;
    unsigned short field1DC;
    char pad1DE[0x72];
    unsigned short flags250;
} RoomResetActor;

typedef struct RoomResetActorEntity {
    unsigned char state;
    char pad1[0xF];
    int *signal;
    char pad14[0x8];
    char *payload;
    char pad20[0x8];
    unsigned char enabled;
} RoomResetActorEntity;

extern RoomResetActor *D_8009D254;
extern volatile int D_800BCF88;

void func_8001AA78(RoomResetActor *actor);
void func_80020CE4(void);
void func_8003E0FC(void *dst, int count, void *src);

int ROOMLIB_RESET_ACTOR_POSITION_NAME(RoomResetActorEntity *entity) {
    int *signal;

    signal = entity->signal;
    if (signal != 0) {
        *signal = 0;
    }

    entity->state = 4;
    if (entity->enabled != 0) {
        {
            RoomResetActorChild *child = D_8009D254->child;

            if (child->active > 0) {
                func_80020CE4();
            }
        }

        {
            RoomResetActor *actor = D_8009D254;

            actor->field1D8 = 0;
            actor->field1DC = 0;
            actor->flags &= 0xFFFEFFFF;
            actor->flags250 &= 0xFBFF;
            func_8003E0FC(entity->payload + 0x1B4, 6, actor->pos);
        }

        {
            RoomResetActor *actor = D_8009D254;

            actor->pos[0] <<= 16;
            actor->pos[2] <<= 16;
            func_8001AA78(actor);
        }

        {
            volatile RoomResetActor *actor;
            int x;
            int y;
            int z;

            actor = D_8009D254;
            x = actor->pos[0];
            y = actor->pos[1];
            z = actor->pos[2];
            actor->velocity[0] = 0;
            actor->velocity[1] = 0;
            actor->velocity[2] = 0;
            actor->movement[0] = 0;
            actor->movement[1] = 0;
            actor->movement[2] = 0;
            actor->basePos[0] = x;
            actor->basePos[1] = y;
            actor->basePos[2] = z;
            D_800BCF88 |= 0x80;
            entity->enabled = 0;
        }
    }

    return 0;
}

#undef ROOMLIB_RESET_ACTOR_POSITION_NAME
