typedef struct { short x,y,z,pad; } Vector;
typedef struct { Vector *position; short frame,reserved; } Particle;
/* Producer func_801947BC limits pending pointers to four. */
typedef struct { Vector *positions[4]; char reserved10[0x46]; short count; } Queue;
typedef struct { int reserved[2]; void *pool; } Emitter;
extern Emitter *D_800F33E0;
extern short D_8019A7FE, D_8019A802;
extern unsigned char D_8019A804;
extern volatile short D_800F3368,D_800F336A,D_800F3376,D_800F3378,D_800F3372,D_800F3374,D_800F336C,D_800F336E;
extern volatile unsigned short D_800E11FA,D_800F3370;
extern unsigned short D_800E2850[];
extern int func_80194F04(int, Particle *);
extern int func_800CE560(void *,int,int,int (*)(int,Particle *));
extern Particle *func_800CE610(void *);
int func_80195064(int event) {
    /* Match note: retail leaves sp+0x10..0x17 unused; original local unknown. */
    char frameGap[8];
    if (event == 1) goto update;
    if (event < 2) {
        if (event == 0) goto setup;
        goto done;
    }
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
                        particle->frame=0;
                    }
                }
            }
            D_8019A7FE=0;
        }
    }
    goto done;
configure:
    {
        int index=D_800E11FA;
        int palette;
        D_800F3368=32;
        D_800F336A=2;
        D_800F3376=32;
        D_800F3378=32;
        D_800F3376=32;
        D_800F3378=32;
        palette=D_800E2850[index];
        asm volatile("" : "=r"(palette) : "0"(palette) : "memory");
        D_800F336C=3;
        D_800F336E=1;
        D_800F3372=0;
        D_800F3374=0;
        D_800F3370=palette;
    }
done:
    return 0;
}
