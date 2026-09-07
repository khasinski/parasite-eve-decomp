#include "pe1/gte.h"
/* Spawns flares and tests the quad swept by the actor's two side endpoints. */
typedef struct { short x,y,z,pad; } Vector;
typedef struct { short rotation[3][3]; int position[3]; } Matrix;
typedef struct { short y,z,shade; } Particle;
typedef struct { unsigned int flags; char reserved04[0x48]; unsigned int status; } Owner;
typedef struct {
    Owner *owner;
    char reserved04[10];
    unsigned char animation;
    char reserved0F[7];
    unsigned short frame;
    char reserved18[0x1D0];
    Matrix transform;
    char reserved208[0x30];
    Matrix *transforms;
} Instance;
typedef struct { int reserved[2]; Instance *instance; } Actor;
typedef struct { int reserved[2]; void *pool; } Emitter;
extern Actor *D_800F32D0;
extern Emitter *D_800F33E0;
extern Instance *g_PlayerEntity;
extern int D_800E27EC;
extern unsigned char D_8019A8C0;
extern short D_8019A898[4],D_8019A8A0[4];
/* Separate field-store views preserve reloads of the complete records. */
extern volatile short rightX asm("D_8019A898");
extern volatile short leftX asm("D_8019A8A0");
extern Vector D_8019A8A8,D_8019A8B0;
extern volatile short D_8019A89C,D_8019A89E,D_8019A8A4;
extern short D_8019A8AE;
extern short D_8019A890[];
extern short D_800F3368,D_800F336A,D_800F3376,D_800F3378;
extern short D_800F336C,D_800F336E,D_800F3372,D_800F3374,D_800F3370;
extern unsigned short D_800E11EA,D_800E2850[];
extern int func_801981D0(int,Particle *);
extern int func_800CE560(void *,int,int,int (*)(int,Particle *));
extern Particle *func_800CE610(void *);
extern int Inv_ScrambleGrid(void);
extern void ApplyMatrixSV(Matrix *,Vector *,Vector *);

int func_80198400(int event) {
    Vector offset,vertices[4];
    int area;
    Instance *instance=D_800F32D0->instance;
    if(event==1) goto update;
    if(event<2) {
        if(event==0) goto setup;
        goto done;
    }
    if(event==2) goto configure;
    goto done;
setup:
    return func_800CE560(D_800F33E0->pool,8,4,func_801981D0);
update:
    if(instance->frame>=31) return 2;
    if(!(D_800E27EC&1)) {
        Particle *particle=func_800CE610(D_800F33E0->pool);
        if(particle) {
            int random=Inv_ScrambleGrid()&255;
            particle->y=random;
            particle->z=(random>>2)+64;
        }
    }
    if(D_8019A8C0 || instance->animation!=9) return 0;
    {
        register Vector *from asm("$5")=(Vector *)&D_8019A898;
        register Vector *to asm("$4")=&D_8019A8A8;
        asm("" : "=r"(from),"=r"(to) : "0"(from),"1"(to));
        *to=*from;
    }
    {
        register Vector *from asm("$5")=(Vector *)&D_8019A8A0;
        register Vector *to asm("$4")=&D_8019A8B0;
        asm("" : "=r"(from),"=r"(to) : "0"(from),"1"(to));
        *to=*from;
    }
    offset.x=256; offset.y=0; offset.z=0;
    ApplyMatrixSV(&instance->transform,&offset,&offset);
    {
        int positionX=instance->transform.position[0];
        unsigned short x;
        unsigned short z;
        int positionZ;
        x=offset.x;
        z=offset.z;
        rightX=x+positionX;
        positionZ=((volatile Instance *)instance)->transform.position[2];
        D_8019A89E=1;
        positionZ=z+positionZ;
        D_8019A89C=positionZ;
        leftX=((volatile Instance *)instance)->transform.position[0]-x;
        D_8019A8A4=((volatile Instance *)instance)->transform.position[2]-z;
    }
    asm volatile("" ::: "memory");
    vertices[0]=*(Vector *)&D_8019A898;
    vertices[1]=*(Vector *)&D_8019A8A0;
    vertices[2]=D_8019A8B0;
    {
        int valid=D_8019A8AE;
        vertices[3]=D_8019A8A8;
        if(!valid) return 0;
        {
            int i=0;
            int previous=((unsigned int)vertices[3].z<<16)|(unsigned short)vertices[3].x;
            int point=((unsigned int)g_PlayerEntity->transform.position[2]<<16)|(unsigned short)g_PlayerEntity->transform.position[0];
            int *out=&area;
            for(;i<4;) {
                int current=((unsigned int)vertices[i].z<<16)|(unsigned short)vertices[i].x;
                gte_ldsxy0(point); gte_ldsxy2(previous); gte_ldsxy1(current);
                gte_nclip(); gte_stmac0(out);
                if(area<0) break;
                i++;
                previous=current;
            }
            if(i<4) return 0;
            {
                Instance *player=g_PlayerEntity;
                asm("" : "=r"(player) : "0"(player));
                D_8019A8C0=1;
                asm volatile("" ::: "memory");
                player->owner->status|=0x4000;
                if(instance->owner) instance->owner->flags|=0x80000000u;
            }
        }
    }
    goto done;
configure:
    {
        short i=0;
        int index,color;
        short *destination=D_8019A890;
        Matrix *transforms;
        transforms=instance->transforms;
        for(;i<3;i++) destination[i]=(transforms[11].position[i]+transforms[15].position[i])>>1;
        index=D_800E11EA;
        D_800F3368=32; D_800F336A=2;
        D_800F3376=32; D_800F3378=32;
        D_800F3376=32; D_800F3378=32;
        color=D_800E2850[index];
        D_800F336C=3; D_800F336E=0;
        D_800F3372=0; D_800F3374=0;
        D_800F3370=color;
    }
done:
    return 0;
}
