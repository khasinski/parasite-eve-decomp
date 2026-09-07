typedef struct { short x,y,z,frame; } Particle;
typedef struct { short rotation[3][3]; int position[3]; } Transform;
typedef struct { char reserved[0x238]; Transform *transforms; } Instance;
typedef struct { int reserved[2]; void *pool; } Emitter;
typedef struct { signed int unused:16; signed int value:16; } SignedHalf;

/* The spawner owns the falling particles updated and rendered by this callback. */
extern Instance *g_PlayerEntity;
extern Emitter *D_800F33E0;
extern int D_800E27EC, D_800F3428, D_8019A464[];
extern unsigned char D_8019A79A,D_8019A79C;
extern unsigned short D_800F336C, D_800E1204[], D_800E2850[];
extern short D_800F336A;
extern short D_800966EE[];
extern volatile short D_800F3368,D_800F3376,D_800F3378,D_800F3372,D_800F3374,D_800F336E;
extern volatile unsigned short D_800E11FA,D_800F3370;
extern int GetClut(int, int);
extern void func_800CEE20(void *, int, int, int, int, unsigned int, int, int, void *);
extern int func_800CE560(void *,int,int,int (*)());
extern Particle *func_800CE610(void *);

int func_801937B4(int event, short *position) {
    if (event == 1) {
        if (D_800E27EC >= 16) return 1;
        position[1] -= position[3]++;
    } else if (event == 2) {
        int kind = D_800F336C;
        register int specialKind asm("$4");
        int texture;
        int handle;
        asm("" : "=r"(kind) : "0"(kind));
        specialKind = 4;
        texture = D_800E1204[kind];
        if (kind == specialKind && D_800F3428) texture += 6;
        else texture += 2;
        handle = GetClut(0, texture);
        func_800CEE20(position, 0, 8192, 8192,
            D_800F336A * (((D_800E27EC - 1) >> 1) & 7),
            (unsigned short)handle, 1,
            ((SignedHalf *)((char *)D_800966EE - 2 +
              (((unsigned int)(D_800E27EC - 1) << 8) & 0x3F00)))->value >> 5,
            D_8019A464);
    }
    return 0;
}

int func_801938E4(int event) {
    if(event==1) goto update;
    if(event<2) { if(event==0) goto setup; goto done; }
    if(event==2) goto configure;
    goto done;
setup:
    return func_800CE560(D_800F33E0->pool,8,4,func_801937B4);
update:
    if(D_8019A79A) return 2;
    if(D_8019A79C) {
        Particle *particle=func_800CE610(D_800F33E0->pool);
        Instance *player;
        if(!particle) return 0;
        player=g_PlayerEntity;
        particle->x=player->transforms->position[0];
        particle->y=player->transforms->position[1];
        {
            int z=player->transforms->position[2];
            particle->frame=0;
            particle->z=z;
        }
        asm volatile("" : : : "memory");
        D_8019A79C=0;
    }
    goto done;
configure:
    {
        int index=D_800E11FA;
        int palette;
        D_800F3368=32; D_800F336A=2;
        D_800F3376=32; D_800F3378=32;
        D_800F3376=32; D_800F3378=32;
        palette=D_800E2850[index];
        asm volatile("" : "=r"(palette) : "0"(palette) : "memory");
        D_800F336C=3; D_800F336E=1;
        D_800F3372=0; D_800F3374=0; D_800F3370=palette;
    }
done:
    return 0;
}
