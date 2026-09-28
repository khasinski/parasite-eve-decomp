#include "pe1/gte.h"
#include "pe1/room_m350_effects.h"
/* Moving effect with collision, queued trail positions and ground rendering. */
extern RoomM350EffectActor *D_800F32D0;
extern RoomM350EffectInstance *g_PlayerEntity;
extern int D_800E27EC,D_800966EC[],D_800F3428;
extern short D_800966EE[],D_8019A7FE,D_8019A800,D_8019A802;
extern unsigned short D_800E1204[],D_800942EC;
extern unsigned short D_800F336C;
extern char D_8019A3C0[],D_8019A4E4[],D_8019A3C8[];
extern int FieldEng_VecToAngle(void *,void *),FieldEng_TurnToward(int,int,int);
extern int Math_IntSqrt(int),GetClut(int,int);
extern void func_800CEE20(void *,int,int,int,int,int,int,int,int);
extern void func_800D004C(void *,int,int,int,void *,int,int,void *,void *,int,int);
int func_801947BC(int event,RoomM350Particle *p)
{
    GteMatrix matrix;
    GteShortVector offset,ground;
    /* Match note: target saves registers 16 bytes later; original local unknown. */
    char frameGap[16];
    if(event==1) {
        int timer=D_800E27EC;
        int brightness;
        if(timer>=48) {
            int result=1;
            unsigned short *count=(unsigned short *)&D_8019A802;
            *count=*count-1;
            return result;
        }
        if(p->tail.value.active.frame>=48) return 0;
        if(!p->tail.value.active.hit) {
            if(p->motion.value.rotation.x>0) p->motion.value.rotation.x-=64;
            else {
                int *trig=D_800966EC;
                int speed=*(short *)((char *)trig+((timer<<6)&0x3FC0))/4;
                int angle=FieldEng_VecToAngle(g_PlayerEntity->transform.t,D_800F32D0->instance->transform.t);
                int yaw=FieldEng_TurnToward(p->motion.value.rotation.y,(short)angle,speed);
                int sum,product;
                p->motion.value.rotation.y=yaw;
                product=(short)trig[(D_800E27EC+2048)&4095]*160;
                sum=yaw+80;
                p->motion.value.rotation.y=sum-product/4096;
            }
            RotMatrixYXZ(&p->motion.value.rotation,&matrix);
            matrix.t[0]=p->position.x;
            matrix.t[1]=p->position.y;
            matrix.t[2]=p->position.z;
            offset.x=0; offset.y=0; offset.z=0u-(unsigned short)p->tail.value.active.speed;
            gte_ldrotmatrix(&matrix); gte_ldtransmatrix(&matrix);
            gte_ldv0(&offset); gte_rtv0tr_mac();
            {
                register int x asm("$12");
                register int y asm("$13");
                register int z asm("$14");
                gte_mfc2_9(x); gte_mfc2_10(y); gte_mfc2_11(z);
                p->position.x=x; p->position.y=y; p->position.z=z;
            }
        }
        if(p->tail.value.active.frame<40) brightness=((short)*(int *)((char *)D_800966EC+((p->tail.value.active.frame<<11)&0x3800))>>6)+128;
        else {
            unsigned int value=*(short *)((char *)D_800966EE+(((p->tail.value.active.frame-40)<<9)&0x3E00));
            brightness=value>>5;
        }
        p->tail.value.active.brightness=brightness;
        if(!p->tail.value.active.hit && g_PlayerEntity->animation>=4) {
            int dx=g_PlayerEntity->transform.t[0]-p->position.x;
            int dz=g_PlayerEntity->transform.t[2]-p->position.z;
            if(Math_IntSqrt((unsigned int)dx*dx+(unsigned int)dz*dz)<128) {
                short *count;
                short n;
                p->tail.value.active.hit=1;
                g_PlayerEntity->owner->status|=0x4000;
                if(D_800F32D0->instance->owner) D_800F32D0->instance->owner->flags|=0x80000000;
                p->tail.value.active.speed=0;
                if(p->tail.value.active.frame<40) p->tail.value.active.frame=40;
                count=&D_8019A7FE; n=*count;
                if(n<4) {
                    register int next asm("$4")=n;
                    asm volatile("" : "=r"(next) : "0"(next));
                    *count=next+1;
                    asm volatile("" : "=r"(count) : "0"(count), "r"(next) : "memory");
                    count-=0x2B;
                    ((RoomM350Particle **)count)[n]=p;
                }
            }
        }
        if(p->tail.value.active.frame>=36 && !(p->tail.value.active.frame&3)) {
            short *count=&D_8019A7FE;
            short n=*count;
            if(n<4) {
                register int next asm("$4")=n;
                asm volatile("" : "=r"(next) : "0"(next));
                *count=next+1;
                asm volatile("" : "=r"(count) : "0"(count), "r"(next) : "memory");
                count-=0x2B;
                ((RoomM350Particle **)count)[n]=p;
            }
        }
        {
            register short *count asm("$4")=&D_8019A800;
            unsigned short n=*(unsigned short *)count;
            int next=n+1;
            int shifted=(unsigned int)n<<16;
            *count=next;
            asm volatile("" : "=r"(count) : "0"(count) : "memory");
            count-=0x24;
            *(GteShortVector *)((shifted>>13)+(unsigned int)count)=p->position;
        }
        p->tail.value.active.frame=(unsigned short)p->tail.value.active.frame+1;
    } else if(event==2) {
        int clut,palette;
        if(p->tail.value.active.frame>=48) return 0;
        palette=D_800E1204[D_800F336C];
        if(D_800F336C==4 && D_800F3428) palette+=4;
        clut=GetClut(32,palette);
        func_800CEE20(p,0,4096,4096,108,(unsigned short)clut,1,p->tail.value.active.brightness,0);
        ground.x=p->position.x; ground.y=D_800942EC; ground.z=p->position.z;
        func_800D004C(&ground,192,192,8,D_8019A3C0,4096,4096,D_8019A4E4,D_8019A3C8,p->tail.value.active.brightness,1);
    }
    return 0;
}


extern RoomM350EffectEmitter *D_800F33E0;
extern GteShortVector D_8019A7A0;
extern unsigned char D_8019A804;
extern volatile short D_800F3368,D_800F336A,D_800F3376,D_800F3378,D_800F3372,D_800F3374,D_800F336E;
extern volatile unsigned short D_800E11EA,D_800F3370;
extern unsigned short D_800E2850[];
extern int func_800CE560(void *,int,int,int (*)(int,RoomM350Particle *));
extern RoomM350Particle *func_800CE610(void *);
int func_80194CFC(int event) {
    if (event == 1) goto update;
    if (event < 2) {
        if (event == 0) goto setup;
        goto done;
    }
    if (event == 2) goto configure;
    goto done;
setup:
    return func_800CE560(D_800F33E0->pool,28,3,func_801947BC);
update:
    {
        RoomM350EffectInstance *instance;
        RoomM350Particle *particle;
        int frame, previous;
        if (D_8019A804) return 2;
        instance=D_800F32D0->instance;
        if (instance->animation != 13) return 0;
        frame=instance->frame;
        previous=instance->previousFrame;
        if (frame<15 || previous>=15) return 0;
        particle=func_800CE610(D_800F33E0->pool);
        *(GteShortVector *)particle=D_8019A7A0;
        particle->position.pad=16;
        particle->motion.value.spawn.speed=512;
        {
            int yaw=D_800F32D0->instance->yaw;
            particle->motion.value.spawn.phase=0;
            particle->motion.value.spawn.yaw=yaw;
        }
        asm volatile("" : : : "memory");
        {
            int count=D_8019A802;
            particle->tail.value.spawn.duration=16;
            particle->tail.value.spawn.frame=0;
            *(volatile unsigned char *)&particle->tail.value.spawn.state=0;
            particle->tail.value.spawn.size=100-count*20;
        }
        asm volatile("" : : : "memory");
        D_8019A802++;
    }
    goto done;
configure:
    {
        int index=D_800E11EA;
        int palette;
        D_800F3368=32;
        D_800F336A=2;
        D_800F3376=32;
        D_800F3378=32;
        D_800F3376=32;
        D_800F3378=32;
        palette=D_800E2850[index];
        asm volatile("" : "=r"(palette) : "0"(palette) : "memory");
        D_800F336C=3;
        D_800F336E=0;
        D_800F3372=0;
        D_800F3374=64;
        D_800F3370=palette;
    }
done:
    return 0;
}
