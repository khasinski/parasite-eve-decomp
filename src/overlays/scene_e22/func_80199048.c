typedef unsigned short u16;
typedef short s16;

extern u16 D_800942EC;
extern char *RoomMain_ActorPtr;
extern s16 D_801994F4, D_801994F6, D_801994F8;
extern int D_801994FC;

void func_800CE870(char *actor, int mode, s16 *position);

s16 *func_80199048(int mode, int value, int unused, int extra)
{
    u16 saved;
    char *actor;
    s16 *out;

    if (mode == 1) {
        saved = D_800942EC;
        actor = RoomMain_ActorPtr;
        out = &D_801994F4;
        *out = value;
        D_801994F8 = extra;
        D_801994F6 = saved;
        func_800CE870(actor, 1, out);
    } else {
        if (value <= 0)
            value = 1;
        D_801994FC = value;
    }

    return &D_801994F4;
}
