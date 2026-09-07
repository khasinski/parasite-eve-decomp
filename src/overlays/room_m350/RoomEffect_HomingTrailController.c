#include "pe1/gte.h"
/* Moving effect with collision, queued trail positions and ground rendering. */
typedef struct { short x,y,z,pad; } Vector;
typedef struct { short rotation[3][3]; int position[3]; } Matrix;
typedef struct {
    Vector position,rotation;
    short speed,reserved12,brightness,frame;
    unsigned char hit,reserved19[3];
} Particle;
typedef struct { unsigned int flags; char reserved04[72]; unsigned int status; } Owner;
typedef struct {
    Owner *owner; char reserved04[10]; unsigned char animation;
    char reserved0F[0x1ED]; int position[3];
} Instance;
typedef struct { int reserved[2]; Instance *instance; } Actor;
extern Actor *D_800F32D0;
extern Instance *g_PlayerEntity;
extern int D_800E27EC,D_800966EC[],D_800F3428;
extern short D_800966EE[],D_8019A7FE,D_8019A800,D_8019A802;
extern unsigned short D_800F336C,D_800E1204[],D_800942EC;
extern char D_8019A3C0[],D_8019A4E4[],D_8019A3C8[];
extern int FieldEng_VecToAngle(void *,void *),FieldEng_TurnToward(int,int,int);
extern int Math_IntSqrt(int),GetClut(int,int);
extern void RotMatrixYXZ(Vector *,Matrix *);
extern void func_800CEE20(void *,int,int,int,int,int,int,int,int);
extern void func_800D004C(void *,int,int,int,void *,int,int,void *,void *,int,int);
int func_801947BC(int event,Particle *p)
{
    Matrix matrix;
    Vector offset,ground;
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
        if(p->frame>=48) return 0;
        if(!p->hit) {
            if(p->rotation.x>0) p->rotation.x-=64;
            else {
                int *trig=D_800966EC;
                int speed=*(short *)((char *)trig+((timer<<6)&0x3FC0))/4;
                int angle=FieldEng_VecToAngle(g_PlayerEntity->position,D_800F32D0->instance->position);
                int yaw=FieldEng_TurnToward(p->rotation.y,(short)angle,speed);
                int sum,product;
                p->rotation.y=yaw;
                product=(short)trig[(D_800E27EC+2048)&4095]*160;
                sum=yaw+80;
                p->rotation.y=sum-product/4096;
            }
            RotMatrixYXZ(&p->rotation,&matrix);
            matrix.position[0]=p->position.x;
            matrix.position[1]=p->position.y;
            matrix.position[2]=p->position.z;
            offset.x=0; offset.y=0; offset.z=0u-(unsigned short)p->speed;
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
        if(p->frame<40) brightness=((short)*(int *)((char *)D_800966EC+((p->frame<<11)&0x3800))>>6)+128;
        else {
            unsigned int value=*(short *)((char *)D_800966EE+(((p->frame-40)<<9)&0x3E00));
            brightness=value>>5;
        }
        p->brightness=brightness;
        if(!p->hit && g_PlayerEntity->animation>=4) {
            int dx=g_PlayerEntity->position[0]-p->position.x;
            int dz=g_PlayerEntity->position[2]-p->position.z;
            if(Math_IntSqrt((unsigned int)dx*dx+(unsigned int)dz*dz)<128) {
                short *count;
                short n;
                p->hit=1;
                g_PlayerEntity->owner->status|=0x4000;
                if(D_800F32D0->instance->owner) D_800F32D0->instance->owner->flags|=0x80000000;
                p->speed=0;
                if(p->frame<40) p->frame=40;
                count=&D_8019A7FE; n=*count;
                if(n<4) {
                    register int next asm("$4")=n;
                    asm volatile("" : "=r"(next) : "0"(next));
                    *count=next+1;
                    asm volatile("" : "=r"(count) : "0"(count), "r"(next) : "memory");
                    count-=0x2B;
                    ((Particle **)count)[n]=p;
                }
            }
        }
        if(p->frame>=36 && !(p->frame&3)) {
            short *count=&D_8019A7FE;
            short n=*count;
            if(n<4) {
                register int next asm("$4")=n;
                asm volatile("" : "=r"(next) : "0"(next));
                *count=next+1;
                asm volatile("" : "=r"(count) : "0"(count), "r"(next) : "memory");
                count-=0x2B;
                ((Particle **)count)[n]=p;
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
            *(Vector *)((shifted>>13)+(unsigned int)count)=p->position;
        }
        p->frame=(unsigned short)p->frame+1;
    } else if(event==2) {
        int clut,palette;
        if(p->frame>=48) return 0;
        palette=D_800E1204[D_800F336C];
        if(D_800F336C==4 && D_800F3428) palette+=4;
        clut=GetClut(32,palette);
        func_800CEE20(p,0,4096,4096,108,(unsigned short)clut,1,p->brightness,0);
        ground.x=p->position.x; ground.y=D_800942EC; ground.z=p->position.z;
        func_800D004C(&ground,192,192,8,D_8019A3C0,4096,4096,D_8019A4E4,D_8019A3C8,p->brightness,1);
    }
    return 0;
}
