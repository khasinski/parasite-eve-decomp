typedef struct { int reserved[2]; int soundMode; } Owner;
typedef struct {
    Owner *owner;
    char reserved04[10];
    unsigned char id;
    char reserved0F[7];
    unsigned short frame;
    char reserved18[2];
    unsigned short previous;
} Animation;
typedef struct { int reserved[2]; Animation *animation; } Actor;
typedef struct { int reserved[2]; void *pool; } Emitter;
typedef struct { short remaining, delay; } State;

extern Actor *D_800F32D0;
extern Emitter *D_800F33E0;
extern short D_8019A7A0[];
extern int func_801944F0(int, void **);
extern int func_800CE560(void *, int, int, int (*)(int, void **));
extern short **func_800CE610(void *);
extern int Asset_Find08w(int, int, short, short, short);

int func_80194654(int event, State *state)
{
    switch (event) {
    case 0:
        state->remaining = 0;
        return func_800CE560(D_800F33E0->pool, 4, 4, func_801944F0);
    case 1:
        {
            Animation *animation = D_800F32D0->animation;
            if (animation->id == 13) {
                int frame = animation->frame;
                if (frame >= 26) {
                    return 2;
                }
                {
                    int previous = animation->previous;
                    if (frame >= 8 && previous < 8) {
                        state->remaining = 4;
                        state->delay = 0;
                    }
                }
            }
            if (state->remaining != 0 && state->delay-- <= 0) {
                short **effect = func_800CE610(D_800F33E0->pool);
                if (effect == 0) {
                    return 0;
                }
                *effect = D_8019A7A0;
                state->delay = 1;
                state->remaining--;
                Asset_Find08w(1479, D_800F32D0->animation->owner->soundMode,
                    (*effect)[0], (*effect)[1], (*effect)[2]);
            }
        }
        break;
    case 2:
        break;
    }
    return 0;
}
