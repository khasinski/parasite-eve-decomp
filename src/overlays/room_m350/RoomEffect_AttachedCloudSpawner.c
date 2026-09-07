/* MASPSX_FLAGS: --expand-div */
typedef struct { short x,y,z,pad; } Vector;
typedef struct { short rotation[3][3]; int position[3]; } Matrix;
typedef struct { int *anchor; short position[3],size; int reserved[2]; } Particle;
typedef struct { char reserved[0x238]; Matrix *transforms; } Instance;
typedef struct { int reserved[2]; Instance *instance; } Actor;
typedef struct { int reserved[2]; void *pool; } Emitter;
extern Actor *D_800F32D0;
extern Emitter *D_800F33E0;
extern unsigned char D_8019A8BE;
extern volatile short D_800F3368,D_800F336A,D_800F3376,D_800F3378,D_800F336C,D_800F336E,D_800F3372,D_800F3374;
extern volatile unsigned short D_800E11E8,D_800F3370;
extern unsigned short D_800E2850[];
extern int func_80198860(int,Particle *);
extern int func_800CE560(void *,int,int,int (*)(int,Particle *));
extern Particle *func_800CE610(void *);
extern int Inv_ScrambleGrid(void);
extern void RotMatrixYXZ(Vector *,Matrix *);
extern void ApplyMatrixSV(Matrix *,Vector *,Vector *);
int func_80198ABC(int event)
{
    Matrix matrix;
    Vector vector;
    if(event==1) goto update;
    if(event<2) { if(event==0) goto setup; goto done; }
    if(event==2) goto configure;
    goto done;
setup:
    return func_800CE560(D_800F33E0->pool,20,16,func_80198860);
update:
    {
        int i;
        if(D_8019A8BE) return 2;
        for(i=0;i<2;i++) {
            register int random asm("$16");
            int divisor;
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
            divisor=128;
            vector.z=random%divisor;
            ApplyMatrixSV(&matrix,&vector,(Vector *)particle->position);
            particle->position[1]=(unsigned short)particle->position[1]*2+128;
            particle->size=((unsigned int)Inv_ScrambleGrid()<<4)+4096;
            particle->anchor=D_800F32D0->instance->transforms[i ? 11:15].position;
        }
    }
    goto done;
configure:
    {
        int unit=16;
        register int index asm("$4")=D_800E11E8;
        int palette;
        D_800F3372=0;
        D_800F3368=unit; D_800F336A=1;
        D_800F3376=unit; D_800F3378=unit;
        D_800F3376=unit; D_800F3378=unit;
        asm volatile("" : "=r"(index) : "0"(index));
        palette=D_800E2850[index];
        D_800F336C=2; D_800F336E=0;
        D_800F3374=unit; D_800F3370=palette;
    }
done:
    return 0;
}
