#include "pe1/gte.h"
/* Animation 11 controller: paired model points and alternating player-facing turn. */
typedef struct { short x,y,z,pad; } Vector;
typedef struct { short rotation[3][3]; int position[3]; } Matrix;
typedef struct { unsigned char state; } Action;
typedef struct { unsigned int flags; char reserved04[20]; Action *action; } Owner;
typedef struct {
    Owner *owner; char reserved04[10]; unsigned char animation,length;
    char reserved10[4]; unsigned int frame;
    char reserved18[34]; short yaw;
    char reserved3C[0x1C0]; Vector position;
    char reserved204[0x34]; Matrix *transforms;
} Instance;
typedef struct { int reserved[2]; Instance *instance; } Actor;
extern Actor *D_800F32D0;
extern Instance *D_8019A774,*g_PlayerEntity;
extern Vector D_8019A778[];
extern short D_8019A77E[],D_8019A798,D_800966EC[];
extern volatile unsigned char D_8019A79A,D_8019A79C;
extern int FieldEng_VecToAngle(Vector *,Vector *);
extern int FieldEng_TurnToward(int,int,int);
int func_80193D6C(int event)
{
    register Instance *instance asm("$16")=D_800F32D0->instance;
    Vector offset;
    if(event==0) {
        Owner *owner=instance->owner;
        unsigned int flags=owner->flags;
        owner->flags=flags|0x40000000;
        asm volatile("" ::: "memory");
        D_8019A798=3; D_8019A79A=0; D_8019A79C=0;
        asm volatile("" ::: "$2");
        return 0;
    }
    if(event==1) {
        short i;
        Vector *input;
        Vector *base;
        int frame;
        short *repeat;
        if(instance->owner && instance->owner->action->state==1)
            instance->owner->action->state=2;
        i=0;
        input=&offset;
        base=D_8019A778;
        offset.x=-112; offset.y=0; offset.z=-80;
        for(;i<2;i++) {
            register Matrix *matrix asm("$2")=D_8019A774->transforms;
            Vector *out;
            if(i) matrix+=15; else matrix+=11;
            {
                register int x asm("$12");
                register int y asm("$13");
                register int z asm("$14");
                volatile int *words=(volatile int *)matrix;
                /* Separate transfers and barriers keep GCC from hoisting the
                 * pad-array address. These are not GTE hardware hazards. */
                x=words[0];
                y=words[1];
                gte_ctc2_0(x);
                gte_ctc2_1(y);
                x=words[2];
                y=words[3];
                z=words[4];
                gte_ctc2_2(x);
                gte_ctc2_3(y);
                asm volatile("" ::: "memory");
                gte_ctc2_4(z);
                asm volatile("" ::: "memory");
                x=words[5];
                asm volatile("" ::: "memory");
                y=words[6];
                asm volatile("" ::: "memory");
                gte_ctc2_5(x);
                asm volatile("" ::: "memory");
                z=words[7];
                asm volatile("" ::: "memory");
                gte_ctc2_6(y);
                asm volatile("" ::: "memory");
                gte_ctc2_7(z);
                asm volatile("" ::: "memory");
            }
            gte_ldv0(input); gte_rtv0tr_mac();
            out=(Vector *)((i*8)+(unsigned int)base);
            {
                register int x asm("$12");
                register int y asm("$13");
                register int z asm("$14");
                gte_mfc2_9(x); gte_mfc2_10(y); gte_mfc2_11(z);
                out->x=x; out->y=y; out->z=z;
            }
            offset.x=0u-(unsigned short)offset.x;
            D_8019A77E[i*4]=i;
        }
        frame=*(unsigned short *)((char *)&instance->frame+2);
        if(instance->animation!=11) return 0;
        repeat=&D_8019A798;
        if(*repeat>0) {
            int speed,bias,angle;
            int signedFrame=(short)frame;
            if(signedFrame>=40) {
                instance->frame=0x30000;
                *repeat=*(volatile unsigned short *)repeat-1;
            }
            if(signedFrame<28) return 0;
            speed=*(short *)((char *)D_800966EC+(((signedFrame-28)*680)&0x3FF8))/32;
            bias=-128;
            if(*(volatile unsigned short *)repeat&1) bias=128;
            angle=FieldEng_VecToAngle(&g_PlayerEntity->position,&instance->position);
            instance->yaw=FieldEng_TurnToward(instance->yaw,(short)(bias+angle),speed);
        } else if((short)frame>=instance->length-1) {
            register Owner *owner asm("$4")=instance->owner;
            if(owner) owner->action->state=4;
            D_8019A79A=1;
            return 1;
        }
    }
    return 0;
}
