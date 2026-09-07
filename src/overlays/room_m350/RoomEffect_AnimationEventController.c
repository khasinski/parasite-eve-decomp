/* Dispatches frame-table events and the actor's turning/recoil motion. */
typedef struct { short x,y,z,pad; } Vector;
typedef struct { short rotation[3][3]; int position[3]; } Matrix;
typedef struct { unsigned char frame,slot,value,pad; } Event;
typedef struct { Event entries[8]; } Events;
typedef struct { char reserved[0x18]; unsigned char *status; } Owner;
typedef struct {
    Owner *owner;
    char reserved04[10];
    unsigned char animation,frameCount;
    char reserved10[4];
    union { int fixed; struct { unsigned short fraction,frame; } parts; } time;
    unsigned short previousFraction,previousFrame;
    char reserved1C[12];
    int localPosition[3];
    short reserved34[3],yaw;
    char reserved3C[0x1AC];
    Matrix transform;
} Instance;
typedef struct { int reserved[2]; Instance *instance; } Actor;
extern Actor *D_800F32D0;
extern Instance *g_PlayerEntity;
extern Events D_8018F1CC;
extern unsigned char D_8019A854[],D_8019A859,D_8019A86E;
extern int D_8019A858;
extern short D_8019A85C,D_8019A85E,D_8019A86A,D_8019A86C;
extern int D_800966EC[];
extern short D_800966EE[];
extern int FieldEng_VecToAngle(int *,int *);
extern int FieldEng_TurnToward(int,int,int);
extern void ApplyMatrixSV(Matrix *,Vector *,Vector *);

int func_80197DA8(int event,short *cursor) {
    Events events=D_8018F1CC;
    Vector movement;
    Instance *instance=D_800F32D0->instance;
    int frame,previous;
    short phase;
    if (event==0) {
        *(int *)D_8019A854=0;
        D_8019A858=0;
        D_8019A86E=0;
        D_8019A85C=0;
        D_8019A86A=0;
        D_8019A86C=0;
        *cursor=0;
        goto done;
    }
    if (event!=1) return 0;
    frame=instance->time.parts.frame;
    previous=instance->previousFrame;
    if (instance->owner && *instance->owner->status==1)
        *instance->owner->status=2;
    {
        short *timer=&D_8019A86C;
        if (*timer) --*timer;
    }
    if (instance->animation==6) {
        if ((short)frame<6) return 0;
        if ((short)previous<6) D_8019A859=5;
        return 0;
    }
    if (D_8019A85E>0) {
        if ((short)frame>=31) {
            instance->time.fixed=0x60000;
            asm volatile("" ::: "memory");
            D_8019A85E--;
        }
    } else if ((short)frame>=instance->frameCount-1) {
        if (instance->owner) *instance->owner->status=4;
        D_8019A86E=1;
        return 1;
    }
    if ((unsigned int)(frame-7)<8) phase=15-frame;
    else if ((unsigned int)(frame-19)<8) phase=27-frame;
    else phase=0;
    if (phase) {
        int speed=(short)D_800966EC[(((unsigned int)phase<<10)&0x3C00)/4]>>6;
        int angle=FieldEng_VecToAngle(g_PlayerEntity->transform.position,instance->transform.position);
        instance->yaw=FieldEng_TurnToward(instance->yaw,(short)angle,speed);
    }
    while ((short)frame>=events.entries[*cursor].frame) {
        Event *entry=&events.entries[*cursor];
        if ((short)previous>=entry->frame) break;
        D_8019A854[entry->slot]=entry->value;
        *cursor+=1;
        if ((unsigned short)*cursor>=8) *cursor=2;
    }
    {
        short *timer=&D_8019A86A;
        int remaining;
        remaining=*timer;
        if (remaining>0) {
            movement.y=0;
            movement.x=0;
            movement.z=D_800966EE[(((8-remaining)<<9)&0x3E00)/2]/256;
            ApplyMatrixSV(&instance->transform,&movement,&movement);
            instance->localPosition[0]+=(unsigned int)(int)movement.x<<16;
            instance->localPosition[2]+=(unsigned int)(int)movement.z<<16;
            --*timer;
        }
    }
done:
    return 0;
}
