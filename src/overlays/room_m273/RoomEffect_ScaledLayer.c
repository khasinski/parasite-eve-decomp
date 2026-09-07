typedef struct { short x,y,z,w; } Vector;
typedef struct { Vector position; unsigned char parameter[4]; } Effect;
typedef struct { unsigned char unknown[0x594]; int x,y,z; } Transform;
typedef struct { unsigned char unknown[0x238]; Transform *transform; } Owner;
typedef struct { unsigned char unknown[8]; Owner *owner; } StateContext;
typedef struct { unsigned char unknown[8]; void *pool; } Context;

/* The emitter owns the twelve-byte records rendered by this callback. */
extern Context *D_800F33E0;
extern StateContext *D_800F32D0;
extern int D_800E27EC;
extern short D_800966EC[],D_800966EE[];
extern unsigned short D_800942EC;
extern unsigned char D_8019AB70[];
extern void func_800D004C(Vector *,int,int,int,Vector *,int,int,void *,void *,int,int);
extern int func_800CE560(void *,int,int,int (*)());
extern Effect *func_800CE610(void *);

int func_8019353C(int mode,Effect *effect) {
    Vector position,rotation;
    /* Layout only: purpose of these eight original frame bytes is unknown. */
    int unknownFrame[2];
    if(mode==1) {
        if(D_800E27EC>=8) return 1;
    } else if(mode==2) {
        short frame=D_800E27EC-1;
        /* The input vector's fourth halfword supplies the initial scale. */
        int product=(*(short *)((char *)D_800966EC+(((unsigned int)frame<<9)&0x3E00))*2+4096)*effect->position.w;
        int sample=*(int *)((char *)D_800966EC+(((unsigned int)frame<<9)&0x3E00));
        short size=product/4096;
        short shade=sample>>21;
        short i;
        rotation.x=1024; rotation.y=0; rotation.z=frame*170; rotation.w=-1;
        position=effect->position;
        i=0;
        do {
            func_800D004C(&position,128,128,12,&rotation,size,size,D_8019AB70,effect->parameter,shade,1);
            shade>>=1;
            size=size*3/2;
            i++;
            position.y=D_800942EC;
        } while(i<2);
    }
    return 0;
}

int func_801936F0(int mode) {
    /* Layout only: purpose of these eight original frame bytes is unknown. */
    int unknownFrame[2];
    switch(mode) {
    case 0:
        return func_800CE560(D_800F33E0->pool,12,9,func_8019353C);
    case 1: {
        Effect *effect;
        Transform *transform;
        int scale;
        if(D_800E27EC>=12) return 2;
        effect=func_800CE610(D_800F33E0->pool);
        if(!effect) break;
        transform=D_800F32D0->owner->transform;
        effect->position.x=transform->x;
        asm("" : : : "memory");
        effect->position.y=transform->y;
        effect->position.z=transform->z;
        scale=D_800966EE[((D_800E27EC*1024/12)&0xFFF)*2];
        effect->parameter[0]=40;
        effect->position.w=scale;
        effect->parameter[1]=D_800966EC[((D_800E27EC*1024/12)&0xFFF)*2]*40/4096;
        effect->parameter[2]=0;
        break;
    }
    case 2:
        break;
    }
    return 0;
}
