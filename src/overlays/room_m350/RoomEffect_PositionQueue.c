typedef struct { short x,y,z,pad; } Vector;
typedef struct { Vector position; short delay,reserved; } Particle;
typedef struct { char reserved[0x3A]; unsigned short yaw; } Instance;
typedef struct { int reserved[2]; Instance *instance; } Actor;
typedef struct { Vector positions[8]; char reserved40[8]; short count; } Queue;
typedef struct { int reserved[2]; void *pool; } Emitter;
typedef struct { signed int unused : 16; signed int value : 16; } SignedHalf;
extern Actor *D_800F32D0;
extern Emitter *D_800F33E0;
extern int D_800E27EC, D_800F3428, D_8019A3D0[];
extern unsigned short D_800F336C, D_800E1204[];
extern short D_800F336A, D_800966EE[];
extern int GetClut(int, int);
extern void func_800CEE20(void *, void *, int, int, int, unsigned int, int, int, void *);
extern short D_8019A800, D_8019A802;
extern unsigned char D_8019A804;
extern volatile short D_800F3368,D_800F3376,D_800F3378,D_800F3372,D_800F3374,D_800F336E;
extern volatile unsigned short D_800E11EA,D_800F3370;
extern unsigned short D_800E2850[];
extern int func_800CE560(void *,int,int,int (*)(int,Particle *));
extern Particle *func_800CE610(void *);

int func_80195218(int event, Particle *object)
{
    short vector[4];
    if (event == 1) {
        if (D_800E27EC >= 16) return 1;
    } else if (event == 2) {
        int frame = D_800E27EC - 2;
        if (frame >= 0) {
            int size,kind,palette,handle;
            vector[0] = 1024; vector[1] = object->position.pad;
            vector[2] = 1024; vector[3] = 1;
            size = D_800966EE[((unsigned int)frame << 7) & 0x1F80];
            kind = D_800F336C; palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428) palette += 4;
            handle = GetClut(0, palette);
            func_800CEE20(object, vector, size, size * 2, D_800F336A * object->delay + 64,
                (unsigned short)handle, 1,
                ((SignedHalf *)((char *)D_800966EE - 2 + (((unsigned int)frame << 8) & 0x3F00)))->value >> 5,
                D_8019A3D0);
        }
    }
    return 0;
}

int func_80195378(int event) {
    char frameGap[8];
    if (event == 1) goto update;
    if (event < 2) { if (event == 0) goto setup; goto done; }
    if (event == 2) goto configure;
    goto done;
setup:
    return func_800CE560(D_800F33E0->pool,12,50,func_80195218);
update:
    if (D_8019A804 && D_8019A802==0) return 2;
    {
        short *count=&D_8019A800;
        if (*count) {
            register int i asm("$16")=0;
            if (*count>0) {
                Queue *queue=(Queue *)((char *)count-(unsigned long)&((Queue *)0)->count);
                Vector *position=queue->positions;
                goto emit;
                for(;i<queue->count;i++,position++) {
emit:
                    {
                        Particle *particle=func_800CE610(D_800F33E0->pool);
                        if (!particle) break;
                        particle->position=*position;
                        particle->position.pad=D_800F32D0->instance->yaw;
                        particle->delay=D_800E27EC & 7;
                    }
                }
            }
            D_8019A800=0;
        }
    }
    goto done;
configure:
    {
        int index=D_800E11EA, palette;
        D_800F3368=32; D_800F336A=2;
        D_800F3376=32; D_800F3378=32; D_800F3376=32; D_800F3378=32;
        palette=D_800E2850[index];
        D_800F336C=3; D_800F336E=0; D_800F3372=0; D_800F3374=0; D_800F3370=palette;
    }
done:
    return 0;
}
