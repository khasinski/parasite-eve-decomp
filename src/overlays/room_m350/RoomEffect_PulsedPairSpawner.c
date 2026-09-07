/* MASPSX_FLAGS: --expand-div */
typedef struct { short x,y,z,pad; } Vector;
typedef struct { short rotation[3][3]; int position[3]; } Matrix;
typedef struct { short position[3],reserved; } Particle;
typedef struct { int reserved[2]; void *pool; } Emitter;
extern Emitter *D_800F33E0;
extern Vector D_8019A778[];
extern int D_800E27EC;
extern short D_800966EE[];
extern volatile short D_800F3368,D_800F336A,D_800F3376,D_800F3378;
extern volatile short D_800F336C,D_800F336E,D_800F3372,D_800F3374;
extern volatile unsigned short D_800E11E8,D_800F3370;
extern unsigned short D_800E2850[];
extern int func_80199B94(int,Particle *);
extern int func_800CE560(void *,int,int,int (*)(int,Particle *));
extern Particle *func_800CE610(void *);
extern int Inv_ScrambleGrid(void);
extern void RotMatrixYXZ(Vector *,Matrix *);
extern void ApplyMatrixSV(Matrix *,Vector *,Vector *);
int func_80192508(int event)
{
    Vector offset;
    Matrix matrix;
    Vector vector;
    /* Match note: original purpose of unused sp+0x40..0x47 is unknown. */
    char frameGap[8];
    if(event==1) goto update;
    if(event<2) { if(event==0) goto setup; goto done; }
    if(event==2) goto configure;
    goto done;
setup:
    return func_800CE560(D_800F33E0->pool,8,64,func_80199B94);
update:
    {
        int timer=D_800E27EC;
        int count;
        int i;
        Vector *out;
        if(timer>=33) return 2;
        count=(D_800966EE[((((unsigned int)timer<<7)+2048)&4095)*2]+4096)/2048+1;
        if(count*2<=0) return 0;
        i=0;
        out=&offset;
        do {
            register int random asm("$16");
            int divisor;
            Vector *base;
            Particle *particle=func_800CE610(D_800F33E0->pool);
            if(!particle) return 0;
            vector.x=(unsigned int)Inv_ScrambleGrid()<<4;
            vector.y=(unsigned int)Inv_ScrambleGrid()<<4;
            vector.z=0;
            RotMatrixYXZ(&vector,&matrix);
            asm volatile("" : : "i"(&&offset_start));
offset_start:
            vector.x=0; vector.y=0;
            random=Inv_ScrambleGrid();
            random=((unsigned int)random<<8)|(unsigned int)Inv_ScrambleGrid();
            divisor=192;
            vector.z=random%divisor+64;
            ApplyMatrixSV(&matrix,&vector,out);
            base=&D_8019A778[i&1];
            particle->position[0]=(unsigned short)offset.x+(unsigned short)base->x;
            particle->position[1]=(unsigned short)offset.y+(unsigned short)base->y;
            particle->position[2]=(unsigned short)offset.z+(unsigned short)base->z;
            i++;
        } while(i<count*2);
    }
    goto done;
configure:
    {
        int index=D_800E11E8;
        int palette;
        D_800F3368=16; D_800F336A=1;
        D_800F3376=16; D_800F3378=16;
        D_800F3376=16; D_800F3378=16;
        palette=D_800E2850[index];
        D_800F336C=2; D_800F336E=0;
        D_800F3372=0; D_800F3374=0; D_800F3370=palette;
    }
done:
    return 0;
}
