/* MASPSX_FLAGS: --expand-div */
/* Event state: rotation, emission count, brightness, fade phase and size. */
typedef struct { short x,y,z,pad; } Vector;
typedef struct { short rotation[3][3]; int position[3]; } Matrix;
typedef struct { Vector rotation; short count,brightness,fade,size; } State;
typedef struct { short x,y,z,size; } Particle;
typedef struct { int reserved[2]; void *asset; } Owner;
typedef struct { Owner *owner; char reserved04[0x234]; Matrix *transforms; } Instance;
typedef struct { int reserved[2]; Instance *instance; } Actor;
typedef struct { int reserved[2]; void *pool; } Emitter;
extern Actor *D_800F32D0;
extern Emitter *D_800F33E0;
extern unsigned char D_8019A82E;
extern int D_800E27EC,D_800966EC[],D_800F3428;
extern unsigned short D_800E2850[],D_800E1204[];
extern volatile unsigned short D_800E11E8,D_800F3370;
extern volatile short D_800F3368,D_800F336A,D_800F3376,D_800F3378,D_800F336C,D_800F336E,D_800F3372,D_800F3374;
extern char D_8019A570[];
extern int Asset_Find08w(int,void *,int,int,int),Inv_ScrambleGrid(void),GetClut(int,int);
extern int func_80195B64(int,Particle *),func_800CE560(void *,int,int,int (*)(int,Particle *));
extern Particle *func_800CE610(void *);
extern void RotMatrixYXZ(Vector *,Matrix *),ApplyMatrixSV(Matrix *,Vector *,void *);
extern void func_800CEE20(void *,void *,int,int,int,int,int,int,void *);
int func_80195CE0(int event,State *inputState)
{
    State *state=inputState;
    union { Matrix matrix; Vector position; } local;
    Vector vector;
    if(event==1) goto update;
    if(event<2) { if(event==0) goto setup; goto done; }
    if(event==2) goto draw;
    goto done;
setup:
    {
        Instance *instance=D_800F32D0->instance;
        int *position=instance->transforms[33].position;
        Emitter *emitter;
        Asset_Find08w(0x5C9,instance->owner->asset,(short)position[0],(short)position[1],(short)position[2]);
        *(int *)&state->rotation=0;
        emitter=D_800F33E0;
        *(int *)((char *)&state->rotation+4)=0;
        state->count=0; state->fade=0;
        return func_800CE560(emitter->pool,8,18,func_80195B64);
    }
update:
    {
        int phase,sample;
        register int i asm("$19");
        if(D_8019A82E) {
            unsigned short old=*(volatile unsigned short *)&state->fade;
            phase=(short)old*64+1024;
            state->fade=old+1;
            if(phase>=2048) return 2;
        } else {
            int timer=D_800E27EC;
            phase=timer<32 ? (unsigned int)timer<<5:1024;
        }
        sample=(short)D_800966EC[phase&4095];
        state->brightness=sample>>5;
        state->size=(unsigned int)sample<<2;
        if(state->count>=16) goto done;
        i=0;
        do {
            register Particle *particle asm("$17")=func_800CE610(D_800F33E0->pool);
            register int random asm("$16");
            int divisor;
            if(!particle) break;
            i++;
            vector.x=(unsigned int)Inv_ScrambleGrid()<<4;
            vector.y=(unsigned int)Inv_ScrambleGrid()<<4; vector.z=0;
            RotMatrixYXZ(&vector,&local.matrix);
            asm volatile("" : : "i"(&&radial));
radial:
            vector.x=0; vector.y=0;
            random=Inv_ScrambleGrid();
            random=((unsigned int)random<<8)|Inv_ScrambleGrid();
            divisor=44;
            vector.z=random%divisor+320;
            ApplyMatrixSV(&local.matrix,&vector,particle);
            particle->size=(unsigned int)state->count*128+2048;
        } while(i<2);
        state->count=(unsigned short)state->count+1;
    }
    goto done;
draw:
    {
        int index=D_800E11E8;
        int i;
        int palette;
        Actor *actor;
        register int specialKind asm("$17");
        register unsigned short *palettes asm("$19");
        volatile int *position;
        short *out;
        D_800F3368=16; D_800F336A=1;
        D_800F3376=16; D_800F3378=16; D_800F3376=16; D_800F3378=16;
        palette=D_800E2850[index];
        asm volatile("" : "=r"(palette) : "0"(palette) : "memory");
        actor=D_800F32D0;
        D_800F336C=2; D_800F336E=0; D_800F3372=0; D_800F3374=0; D_800F3370=palette;
        asm volatile("" : "=r"(actor) : "0"(actor) : "memory");
        i=0;
        position=actor->instance->transforms[33].position;
        out=&local.position.x;
        for(;i<3;i++) *out++=*position++;
        i=0;
        palettes=D_800E1204;
        state->rotation.z=(unsigned int)D_800E27EC<<8;
        specialKind=4;
        do {
            int kind=*(unsigned short *)&D_800F336C;
            int clut,pal=palettes[kind];
            if(kind==specialKind && D_800F3428) pal+=4;
            clut=GetClut(32,pal);
            func_800CEE20(&local.position,state,state->size,state->size,*(short *)&D_800F336A+216,(unsigned short)clut,1,state->brightness,D_8019A570);
            state->rotation.z=0u-(unsigned short)state->rotation.z;
        } while(++i<2);
    }
done:
    return 0;
}
