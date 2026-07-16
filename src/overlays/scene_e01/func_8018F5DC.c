typedef unsigned short u16;
typedef unsigned int u32;

typedef struct {
    char pad0[0x28];
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
    char pad0[0x10];
    char *payload;
    char pad14[0x8];
    unsigned char enabled;
} EntityState;

extern Actor *D_8009D254;
extern int D_800BCF88[];

void func_8001AA78(Actor *actor);
void func_8003E0FC(void *dst, int count, void *src);

void func_8018F5DC(void *unused, EntityState *state) {
    {
        Actor *actor = D_8009D254;

        actor->field_1D8 = 0;
        actor->field_1DC = 0;
        actor->flags &= 0xFFFEFFFF;
        actor->h250 &= 0xFBFF;
        func_8003E0FC(state->payload + 0x1B4, 6, actor->pos);
    }

    {
        Actor *actor = D_8009D254;

        actor->pos[0] <<= 16;
        actor->pos[2] <<= 16;
        func_8001AA78(actor);
    }

    {
        Actor *actor = D_8009D254;
        int x = actor->pos[0];
        int y = actor->pos[1];
        int z = actor->pos[2];
        int *flags = D_800BCF88;

        actor->vel[0] = 0;
        actor->vel[1] = 0;
        actor->vel[2] = 0;
        actor->move[0] = 0;
        actor->move[1] = 0;
        actor->move[2] = 0;
        actor->base_pos[0] = x;
        actor->base_pos[1] = y;
        actor->base_pos[2] = z;
        *flags |= 0x80;
        state->enabled = 0;
    }
}
