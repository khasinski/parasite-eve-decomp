typedef struct { short x,y,z,pad; } Vector;
typedef struct { short rotation[3][3]; int position[3]; } Matrix;
typedef struct { unsigned char state; } Action;
typedef struct { int reserved[2]; int asset; char reservedC[12]; Action *action; } Owner;
typedef struct {
    Owner *owner; char reserved04[10]; unsigned char animation,length;
    char reserved10[4]; unsigned int frame;
    char reserved18[16]; int position[3]; char reserved34[6]; short yaw;
    char reserved3C[0x1FC]; Matrix *transforms;
} Instance;
typedef struct { int reserved[2]; Instance *instance; } Actor;
typedef struct { int asset; short speed,targetSpeed,heightPhase,mode; } State;
extern Actor *D_800F32D0;
extern Instance *g_PlayerEntity;
extern volatile unsigned char D_8019A8BE,D_8019A8C0;
extern short D_8019A89E,D_8019A8B8,D_8019A8BA,D_8019A8BC;
extern short D_800942EC,D_800966EC[];
extern volatile short D_8019A870,D_8019A872,D_8019A874,D_8019A876,D_8019A878,D_8019A87A,D_8019A87C,D_8019A87E,D_8019A880;
extern volatile int D_8019A884,D_8019A888,D_8019A88C;
extern int Asset_Find08w(int,int,int,int,int);
extern void Akao_Cmd_21(int,int);
extern int func_800DFE20(int *,int *);
extern int FieldEng_VecToAngle(int *,int *);
extern int FieldEng_TurnToward(int,int,int);
extern int rcos(int);
extern int rsin(int);
extern void ApplyMatrixSV(Matrix *,Vector *,Vector *);
int func_80198CF8(int event,State *state)
{
    Instance *instance=D_800F32D0->instance;
    Vector offset;
    /* Match note: original purpose of unused sp+0x20..0x2F is unknown. */
    char frameGap[16];
    if(event==0) {
        Matrix *matrix;
        state->speed=0; state->targetSpeed=0; state->heightPhase=0; state->mode=0;
        D_8019A8BE=0; D_8019A8C0=0; D_8019A89E=0;
        asm volatile("" : : : "memory");
        matrix=instance->transforms;
        state->asset=Asset_Find08w(0x5CB,instance->owner->asset,
            (short)matrix->position[0],(short)matrix->position[1],(short)matrix->position[2]);
    } else if(event==1) {
        int frame,angle,delta,cosine,sine;
        int phase;
        int target;
        register int speed asm("$5");
        short *repeat;
        if(instance->animation!=9) return 0;
        if(instance->owner && instance->owner->action->state==1)
            instance->owner->action->state=2;
        repeat=&D_8019A8B8;
        {
            register int current asm("$3")=*(unsigned short *)((char *)&instance->frame+2);
            frame=current;
        if(*repeat>0) {
            register int signedFrame asm("$2")=(short)current;
            if(signedFrame>=30) {
                instance->frame=0x50000;
                *repeat=*(volatile unsigned short *)repeat-1;
            }
        } else if((short)current>=instance->length-1) {
            if(instance->owner) instance->owner->action->state=4;
            D_8019A8BE=1;
            asm volatile("" : : : "memory");
            Akao_Cmd_21(state->asset,0);
            return 1;
        }
        }
        D_8019A8BA=func_800DFE20(g_PlayerEntity->position,instance->position);
        angle=FieldEng_VecToAngle(g_PlayerEntity->position,instance->position)&4095;
        D_8019A8BC=angle;
        asm volatile("" : : : "memory");
        delta=(angle-(unsigned short)instance->yaw)&4095;
        if(state->mode==0) goto accelerate;
        if(state->mode==1) goto steer;
        state->targetSpeed=0;
        goto adjust;
accelerate:
        {
            if(state->speed<=1280) instance->yaw=FieldEng_TurnToward(instance->yaw,angle,80);
            if(state->speed<=state->targetSpeed && ((instance->yaw-D_8019A8BC+64)&4095)<=256) {
                state->mode=1; state->targetSpeed=1536; D_8019A8C0=0;
            }
        }
        goto adjust;
steer:
        {
            if(state->targetSpeed>=state->speed && ((delta-1536)&4095)<1024) {
                state->mode=0; state->targetSpeed=512;
            }
        }
adjust:
        speed=state->speed; target=state->targetSpeed;
        {
            register int next asm("$3")=speed;
            int result;
            if(speed<target) {
                result=next+128;
                goto store_speed;
            }
            result=target<speed;
            if(result) {
                result=next-32;
store_speed:
                state->speed=result;
            }
        }
        cosine=rcos(instance->yaw+2048);
        sine=rsin(instance->yaw+2048);
        {
        register Matrix *matrix asm("$4")=(Matrix *)&D_8019A870;
        register Vector *vector asm("$5")=&offset;
        *(volatile short *)matrix=cosine; D_8019A874=sine; D_8019A87C=0u-(unsigned int)sine;
        D_8019A880=cosine;
        D_8019A88C=0; D_8019A888=0; D_8019A884=0;
        D_8019A87E=0; D_8019A87A=0; D_8019A876=0; D_8019A872=0;
        D_8019A878=4096;
        asm volatile("" : : "r"(matrix), "r"(vector) : "memory");
        offset.x=0; offset.y=0; offset.z=state->speed;
        ApplyMatrixSV(matrix,vector,vector);
        }
        instance->position[0]=(unsigned int)instance->position[0]+((unsigned int)offset.x<<12);
        instance->position[2]=(unsigned int)instance->position[2]+((unsigned int)offset.z<<12);
        phase=state->heightPhase;
        {
        int nextPhase=phase;
        asm volatile("" : "=r"(nextPhase) : "0"(nextPhase), "r"(phase));
        if(phase<16) {
            int sample,floor;
            register int height asm("$2");
            phase=nextPhase+1;
            state->heightPhase=phase;
            sample=*(short *)((char *)D_800966EC+(((unsigned int)phase<<8)&0x3F00));
            height=4096-sample;
            floor=D_800942EC;
            height=height/8-128;
            height=floor-height;
            instance->position[1]=(unsigned int)height<<16;
        } else if((short)frame>=31) {
            int floor;
            state->mode=2;
            floor=(unsigned int)D_800942EC<<16;
            if(floor<instance->position[1]) instance->position[1]=(unsigned int)instance->position[1]-0x100000;
            else instance->position[1]=floor;
        }
        }
    }
    return 0;
}
