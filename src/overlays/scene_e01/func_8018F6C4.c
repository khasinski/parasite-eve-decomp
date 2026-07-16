typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

typedef struct {
    char pad0[0xC];
    short h0C;
} ActorChild;

typedef struct {
    ActorChild *child;
    char pad4[0x24];
    int pos[3];
    char pad34[0xC];
    int base_pos[3];
    char pad4C[0x1C];
    int vel[3];
    char pad74[0x4];
    int move[3];
    char pad84[0x14];
    u32 flags;
    char pad9C[0x13C];
    int field_1D8;
    u16 field_1DC;
    char pad1DE[0x72];
    u16 h250;
} Actor;

typedef struct {
    u8 state;
    char pad1[0xF];
    int *signal;
    char pad14[0x8];
    char *payload;
    char pad20[0x8];
    u8 enabled;
} Entity;

extern Actor *D_8009D254;
extern volatile int D_800BCF88;

void func_8001AA78(Actor *actor);
void func_80020CE4(void);
void func_8003E0FC(void *dst, int count, void *src);

int func_8018F6C4(Entity *ent) {
    int *signal;

    signal = ent->signal;
    if (signal != 0) {
        *signal = 0;
    }

    ent->state = 4;
    if (ent->enabled != 0) {
        {
            ActorChild *child = D_8009D254->child;

            if (child->h0C > 0) {
                func_80020CE4();
            }
        }

        {
            Actor *actor = D_8009D254;

            actor->field_1D8 = 0;
            actor->field_1DC = 0;
            actor->flags &= 0xFFFEFFFF;
            actor->h250 &= 0xFBFF;
            func_8003E0FC(ent->payload + 0x1B4, 6, actor->pos);
        }

        {
            Actor *actor = D_8009D254;

            actor->pos[0] <<= 16;
            actor->pos[2] <<= 16;
            func_8001AA78(actor);
        }

        {
            volatile Actor *actor;
            int x;
            int y;
            int z;

            actor = D_8009D254;
            x = actor->pos[0];
            y = actor->pos[1];
            z = actor->pos[2];
            actor->vel[0] = 0;
            actor->vel[1] = 0;
            actor->vel[2] = 0;
            actor->move[0] = 0;
            actor->move[1] = 0;
            actor->move[2] = 0;
            actor->base_pos[0] = x;
            actor->base_pos[1] = y;
            actor->base_pos[2] = z;
            D_800BCF88 |= 0x80;
            ent->enabled = 0;
        }
    }

    return 0;
}
