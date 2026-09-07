typedef struct { short x, y, z, pad; } Vector;
typedef struct { short rotation[3][3]; int translation[3]; } Matrix;
typedef struct { Vector velocity; short x, y, z; unsigned char stopped, pad; } Particle;
typedef struct { int reserved[2]; void *pool; } Emitter;
typedef struct { int reserved[2]; int soundMode; } Owner;
typedef struct { char reserved[0xF4]; int position[3]; } Transform;
typedef struct {
    Owner *owner;
    char reserved04[0x3A - sizeof(Owner *)];
    unsigned short yaw;
    char reserved3C[0x1FC];
    Transform *transform;
} Instance;
typedef struct { int reserved[2]; Instance *instance; } Actor;
typedef struct { char reserved[0x1FC]; int position[3]; } Player;
extern Emitter *D_800F33E0;
extern Actor *D_800F32D0;
extern Player *g_PlayerEntity;
extern unsigned char D_8019A86E, D_8019A855;
extern short D_8019A86A;
extern int func_80196F2C(int, Particle *);
extern int func_800CE560(void *, int, int, int (*)(int, Particle *));
extern Particle *func_800CE610(void *);
extern int Math_IntSqrt(int);
extern int Gte_Atan2(int, int);
extern int Inv_ScrambleGrid(void);
extern Matrix *RotMatrixYXZ(Vector *, Matrix *);
extern Vector *ApplyMatrixSV(Matrix *, Vector *, Vector *);
extern int Asset_Find08w(int, int, short, short, short);

int func_80197364(int event)
{
    Matrix matrix;
    Vector vector;
    if (event == 1) goto update;
    if (event < 2) {
        if (event == 0) goto setup;
        goto done;
    }
    goto done;
setup:
    return func_800CE560(D_800F33E0->pool, 16, 4, func_80196F2C);
update:
    {
        Particle *effect;
        Instance *instance;
        Transform *transform;
        Player *player;
        int dx, dz, dy;
        int angle;
        if (D_8019A86E) return 2;
        if (!D_8019A855) return 0;
        effect = func_800CE610(D_800F33E0->pool);
        if (!effect) return 0;
        instance = D_800F32D0->instance;
        transform = instance->transform;
        player = g_PlayerEntity;
        effect->x = transform->position[0];
        effect->y = transform->position[1];
        effect->z = transform->position[2];
        effect->stopped = 0;
        dx = transform->position[0] - player->position[0];
        dz = transform->position[2] - player->position[2];
        dy = transform->position[1] - player->position[1];
        angle = Gte_Atan2(dy, Math_IntSqrt(dx * dx + dz * dz));
        angle += (signed char)Inv_ScrambleGrid() / 2;
        angle &= 0xFFF;
        vector.x = angle;
        if (angle < 0xD00) vector.x = 0xD00;
        {
            int offset = (signed char)Inv_ScrambleGrid();
            Vector *v = &vector;
            register Vector *input asm("$4") = v;
            register Matrix *m asm("$5") = &matrix;
            int yaw = instance->yaw;
            /* Keep the rotation arguments ready before adding the yaw offset. */
            asm("" : "=r"(offset) : "0"(offset), "r"(input), "r"(m), "r"(yaw));
            offset += 0x800;
            yaw += offset;
            vector.y = yaw;
            vector.z = 0;
            RotMatrixYXZ(input, m);
        }
        vector.x = 0;
        vector.y = 0;
        vector.z = 128;
        ApplyMatrixSV(&matrix, &vector, &effect->velocity);
        {
            Actor *actor = D_800F32D0;
            D_8019A855 = 0;
            /* Publish consumption before loading the sound's position and owner. */
            asm volatile("" : : : "memory");
            {
                int x = effect->x;
                Asset_Find08w(0x60A, actor->instance->owner->soundMode,
                    x, effect->y, effect->z);
            }
        }
        D_8019A86A = 8;
    }
done:
    return 0;
}
