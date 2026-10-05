#include "room_m350_turn_sequence.h"
#include "pe1/gte.h"
typedef RoomM350Vector Vector;
typedef RoomM350TransformMatrix Matrix;
typedef RoomM350Action Action;
typedef RoomM350TurnSequenceOwner Owner;
typedef RoomM350Instance Instance;
typedef RoomM350Actor Actor;
extern Actor *D_800F32D0;
extern Instance *D_8019A7F8,*g_PlayerEntity;
extern Vector D_8019A7A0;
extern short D_8019A7FC;
extern short D_8019A7FE,D_8019A800,D_8019A802;
extern unsigned char D_8019A804,D_8019A805,D_8019A806;
extern int D_800966EC[];
extern int FieldEng_VecToAngle(Vector *,Vector *);
extern int FieldEng_TurnToward(int,int,int);
int func_801958DC(int event) {
    Instance *instance=D_800F32D0->instance;
    Vector offset;
    if(event==0) {
        register int one asm("$2")=1;
        {
            Owner *owner=instance->owner;
            register unsigned int flags asm("$2")=owner->flags;
            flags |= 0x40000000;
            owner->flags=flags;
        }
        asm volatile("" : : : "memory");
        one=2;
        D_8019A7FC=one;
        D_8019A804=0; D_8019A805=0; D_8019A806=0;
        D_8019A7FE=0; D_8019A800=0; D_8019A802=0;
        asm volatile("" : : : "$2");
        return 0;
    }
    if(event==1) {
        Matrix *matrix;
        int frame;
        int repetitions;
        short *repeat;
        if(instance->owner && instance->owner->action->state==1)
            instance->owner->action->state=2;
        matrix=&D_8019A7F8->transforms[17];
        offset.x=0; offset.y=-144; offset.z=128;
        gte_ldrotmatrix(matrix);
        gte_ldtransmatrix(matrix);
        gte_ldv0(&offset);
        gte_rtv0tr_mac();
        gte_stsv(&D_8019A7A0);
        if(instance->animation!=13) return 0;
        repeat=&D_8019A7FC;
        repetitions=*repeat;
        frame=*(unsigned short *)((char *)&instance->frame+2);
        if(repetitions>0) {
            register int speed asm("$16");
            int angle;
            frame=(short)frame;
            if(frame>=25) {
                instance->frame=0x60000;
                *repeat=*(volatile unsigned short *)repeat-1;
            }
            if(frame<19) return 0;
            speed=*(short *)&D_800966EC[((frame-19)*341)&4095];
            speed/=32;
            angle=FieldEng_VecToAngle(&g_PlayerEntity->position,&instance->position);
            instance->yaw=FieldEng_TurnToward(instance->yaw,(short)angle,speed);
        } else {
            if((short)frame>=instance->length-1) {
                if(instance->owner) instance->owner->action->state=4;
                D_8019A804=1;
                return 1;
            }
        }
    }
    return 0;
}
