typedef struct { signed int size : 16; signed int shade : 16; } TableEntry;
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

extern int D_800E27EC, D_800966EC[], D_8019A3C8[], D_8019A4DC[], D_8019A4E0[];
extern void func_800D004C(void *, int, int, int, short *, int, int, void *, void *, int, int);
extern void func_800D0728(void *, int, int, int, short *, int, int, void *, void *, int, int);
extern Actor *D_800F32D0;
extern Emitter *D_800F33E0;
extern short D_8019A7A0[];
extern int func_800CE560(void *, int, int, int (*)(int, void **));
extern short **func_800CE610(void *);
extern int Asset_Find08w(int, int, short, short, short);

int func_801944F0(int event, void **object)
{
    short vector[4];
    if (event == 1) {
        if (D_800E27EC >= 8) {
            return 1;
        }
    } else if (event == 2) {
        int *base = D_800966EC;
        int counter = D_800E27EC;
        TableEntry *entry = (TableEntry *)((char *)base +
            (((unsigned int)(counter - 1) << 9) & 0x3E00));
        int size = entry->size;

        vector[0] = 0;
        vector[1] = 0;
        vector[2] = counter * 128;
        vector[3] = 0;
        func_800D004C(*object, 320, 320, 16, vector, size, size,
            D_8019A4DC, D_8019A3C8, entry->shade >> 5, 1);
        {
            int nextCounter = D_800E27EC;
            size = nextCounter * 512 + 2048;
            func_800D0728(*object, 256, 512, 16, vector, size, size,
                D_8019A3C8, D_8019A4E0,
                (short)base[((unsigned int)(nextCounter - 1) << 8) & 0xF00] >> 5, 1);
        }
    }
    return 0;
}

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
