typedef struct { short x,y,z,scale; unsigned char parameter[4]; } Effect;
typedef struct { unsigned char unknown[0x594]; int x,y,z; } Transform;
typedef struct { unsigned char unknown[0x238]; Transform *transform; } Owner;
typedef struct { unsigned char unknown[8]; Owner *owner; } StateContext;
typedef struct { unsigned char unknown[8]; void *pool; } Context;
extern Context *D_800F33E0;
extern StateContext *D_800F32D0;
extern int D_800E27EC;
extern short D_800966EC[],D_800966EE[];
extern int func_8019353C();
extern int func_800CE560(void *,int,int,int (*)());
extern Effect *func_800CE610(void *);

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
        effect->x=transform->x;
        asm("" : : : "memory");
        effect->y=transform->y;
        effect->z=transform->z;
        scale=D_800966EE[((D_800E27EC*1024/12)&0xFFF)*2];
        effect->parameter[0]=40;
        effect->scale=scale;
        effect->parameter[1]=D_800966EC[((D_800E27EC*1024/12)&0xFFF)*2]*40/4096;
        effect->parameter[2]=0;
        break;
    }
    case 2:
        break;
    }
    return 0;
}
