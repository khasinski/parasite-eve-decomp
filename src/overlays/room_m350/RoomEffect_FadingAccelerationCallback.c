extern int D_800F3428;
extern unsigned short D_800F336C, D_800E1204[];
extern short D_800F336A;
extern int GetClut(int, int);
extern void func_800CEE20(void *, int, int, int, int, unsigned int, int, int, void *);

typedef struct {
    short x, y, z, unused;
    short velocity, acceleration, brightness, size, phase, counter;
} Particle;

int func_80199148(int event, Particle *object)
{
    register int update asm("$2") = 1;
    /* Retail initializes two unused stack words; their original type is unknown. */
    struct { int x, y; } unused = {0, 0};

    if (event == update) {
        short phase = object->counter >> 1;
        object->phase = phase;
        if (phase >= 8) {
            return 1;
        }
        if (phase >= 4) {
            object->brightness -= 8;
        }
        {
            unsigned int counter = (unsigned short)object->counter;
            register short velocity;
            velocity = object->velocity >> 1;
            object->counter = counter + 1;
            object->y -= velocity;
            object->velocity += object->acceleration;
        }
    } else if (event == 2) {
        int kind = D_800F336C;
        int palette;
        int handle;
        register int special asm("$4");

        asm("" : "=r"(kind) : "0"(kind));
        special = 4;
        palette = D_800E1204[kind];
        if (kind == special && D_800F3428) {
            palette += 6;
        } else {
            palette += 2;
        }
        handle = GetClut(0, palette);
        func_800CEE20(object, 0, object->size, object->size, D_800F336A * object->phase,
            (unsigned short)handle, 1, object->brightness, 0);
    }
    return 0;
}
