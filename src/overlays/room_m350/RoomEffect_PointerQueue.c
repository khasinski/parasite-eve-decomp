typedef struct { short x,y,z,pad; } Vector;
typedef struct { Vector *position; short offset,reserved; } Particle;
typedef struct { Vector *positions[4]; char reserved10[0x46]; short count; } Queue;
typedef struct { int reserved[2]; void *pool; } Emitter;
extern int D_800E27EC, D_800F3428, D_800966EC[], D_8019A4E8[];
extern unsigned short D_800F336C, D_800E1204[];
extern short D_800F336A;
extern int GetClut(int, int);
extern void func_800CEE20(void *, int, int, int, int, unsigned int, int, int, void *);
extern Emitter *D_800F33E0;
extern short D_8019A7FE, D_8019A802;
extern unsigned char D_8019A804;
extern volatile short D_800F3368,D_800F3376,D_800F3378,D_800F3372,D_800F3374,D_800F336E;
extern volatile unsigned short D_800E11FA,D_800F3370;
extern unsigned short D_800E2850[];
extern int func_800CE560(void *,int,int,int (*)(int,Particle *));
extern Particle *func_800CE610(void *);

int func_80194F04(int event, Particle *object)
{
    Vector position;
    if (event == 1) {
        if (D_800E27EC >= 16) return 1;
        object->offset -= 16;
    } else if (event == 2) {
        int frame,kind,palette,handle;
        register int special asm("$3");
        position = *object->position;
        position.y += object->offset;
        kind = D_800F336C;
        asm("" : "=m"(D_800E27EC) : "m"(D_800E27EC), "m"(position.y));
        frame = D_800E27EC - 1;
        asm("" : "=r"(kind) : "0"(kind), "r"(frame));
        special = 4;
        palette = D_800E1204[kind];
        if (kind == special && D_800F3428) palette += 6;
        else palette += 2;
        handle = GetClut(0, palette);
        func_800CEE20(&position, 0, 8192, 8192, D_800F336A * (frame >> 1),
            (unsigned short)handle, 1,
            (short)D_800966EC[((unsigned int)frame << 7) & 0xF80] >> 5,
            D_8019A4E8);
    }
    return 0;
}

int func_80195064(int event) {
    char frameGap[8];
    if (event == 1) goto update;
    if (event < 2) { if (event == 0) goto setup; goto done; }
    if (event == 2) goto configure;
    goto done;
setup:
    return func_800CE560(D_800F33E0->pool,8,16,func_80194F04);
update:
    if (D_8019A804 && D_8019A802==0) return 2;
    {
        short *count=&D_8019A7FE;
        if (*count) {
            register int i asm("$16")=0;
            if (*count>0) {
                Queue *queue=(Queue *)((char *)count-(unsigned long)&((Queue *)0)->count);
                Vector **position=queue->positions;
                goto emit;
                for(;i<queue->count;i++,position++) {
emit:
                    {
                        Particle *particle=func_800CE610(D_800F33E0->pool);
                        if (!particle) break;
                        particle->position=*position;
                        particle->offset=0;
                    }
                }
            }
            D_8019A7FE=0;
        }
    }
    goto done;
configure:
    {
        int index=D_800E11FA, palette;
        D_800F3368=32; D_800F336A=2;
        D_800F3376=32; D_800F3378=32; D_800F3376=32; D_800F3378=32;
        palette=D_800E2850[index];
        asm volatile("" : "=r"(palette) : "0"(palette) : "memory");
        D_800F336C=3; D_800F336E=1; D_800F3372=0; D_800F3374=0;
        D_800F3370=palette;
    }
done:
    return 0;
}
