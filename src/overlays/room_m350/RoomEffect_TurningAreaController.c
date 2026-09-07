#include "pe1/gte.h"

/* Steers a rotating footprint, tests contact, and draws three model/sprite layers. */
typedef struct { short x,y,z,pad; } Vector;
typedef struct { short rotation[3][3]; int position[3]; } Matrix;
typedef struct { int x,y,z,pad; } Scale;
typedef struct { void *asset; int yaw; short size,brightness,unknown,turn; unsigned char initialized; } State;
typedef struct { unsigned int flags; char reserved04[0x48]; unsigned int status; } Owner;
typedef struct {
    Owner *owner; char reserved04[0x24]; int localPosition[3];
    char reserved34[0x1C8]; int position[3]; char reserved208[0x30]; Matrix *transforms;
} Instance;
typedef struct { int reserved[2]; Instance *instance; } Actor;
extern Actor *D_800F32D0;
extern Instance *g_PlayerEntity;
extern void *D_800B0E64;
extern short D_8019A82A;
extern unsigned char D_8019A831,D_8019A82F,D_8019A830;
extern int D_800E27EC,D_800F3428;
extern short D_800966EC[],D_800966EE[];
extern Matrix D_8019A808;
extern short D_8019A80A,D_8019A80C,D_8019A80E,D_8019A810,D_8019A812,D_8019A814,D_8019A816,D_8019A818;
extern int D_8019A81C,D_8019A820,D_8019A824;
extern Vector D_8019A574[];
extern char D_8019A594[],D_8019A3C8[];
extern unsigned short D_800E11FA,D_800E2850[],D_800F336C,D_800E1204[];
extern void *Asset_FindTable08ByU32Key(void *,unsigned int);
extern void func_800C6D5C(void *,int,int);
extern int func_800DFE20(int *,int *),FieldEng_VecToAngle(int *,int *),rcos(int),rsin(int);
extern int GetTPage(int,int,int,int),GetClut(int,int);
extern void GsSetOrign(int,int),func_800C6ED8(int),func_800C6EF8(void *),func_800C6FA0(void *,int);
extern void memset(void *,int,int),Gte_ScaleMatrix(Matrix *,Scale *),func_800C71E4(void *,Matrix *),func_800C6F4C(void *);
extern void func_800D0E88(Vector *,Vector *,int,int,void *,void *,void *,int,int);
int func_801960C4(int event,State *state)
{
    union { struct { Vector vertices[4]; int area; } collision; Matrix matrix; } local;
    Matrix base;
    Vector position,rotation;
    Scale scale;
    Instance *instance=D_800F32D0->instance;
    if(event==1) goto update;
    if(event<2) { if(event==0) goto setup; goto done; }
    if(event==2) goto draw;
    goto done;
setup:
    {
        int yaw;
        register int result asm("$2");
        state->asset=Asset_FindTable08ByU32Key(D_800B0E64,0xC54E0704);
        func_800C6D5C(state->asset,0,0);
        state->size=0;
        asm volatile("" : : : "memory");
        yaw=D_8019A82A;
        asm volatile("" : "=r"(yaw) : "0"(yaw) : "memory");
        result=0;
        state->initialized=0;
        asm volatile("" : : : "memory");
        state->yaw=(unsigned int)yaw<<8;
        return result;
    }
update:
    {
        int timer=D_800E27EC,size;
        register int high asm("$9");
        register int quotient asm("$2");
        int intermediate;
        if(timer<16) size=((4096-*(short *)((char *)D_800966EC+(((unsigned int)timer<<8)&0x3F00)))>>1)+2048;
        else if(timer>=91) {
            int sample=*(short *)((char *)D_800966EE+(((unsigned int)(timer-90)<<8)&0x3F00));
            asm volatile("" : "=r"(sample) : "0"(sample));
            size=sample>>1;
        }
        else size=2048;
        /* Expose the high product; the final sign correction still implements /3. */
        intermediate=size/3+(size>>31);
        asm("" : "=h"(intermediate) : "0"(intermediate));
        high=intermediate;
        asm volatile("" : "=r"(high) : "0"(high), "r"(size>>31));
        quotient=high-(size>>31);
        state->size=quotient;
        if(state->size<=0) return 1;
        state->brightness=size<=2048 ? size>>4:(4096-size)>>4;
        if(!state->initialized) {
            if(D_8019A831) {
                int distance=func_800DFE20(g_PlayerEntity->localPosition,instance->localPosition);
                int angle=FieldEng_VecToAngle(g_PlayerEntity->localPosition,instance->localPosition);
                int yaw=state->yaw;
                int rawDelta=((unsigned int)angle-(yaw>>8))&4095;
                register int delta asm("$4")=rawDelta;
                int speed,factor;
                if(rawDelta<512) { state->yaw=(unsigned int)yaw-0x20000; delta=rawDelta+512; }
                if((short)delta>3584) { delta-=512; state->yaw=(unsigned int)state->yaw+0x20000; }
                if((short)delta>2048) delta=4096-delta;
                speed=(short)distance-512;
                if(speed<0) speed=0;
                factor=1536; if(speed>1024) factor=1024;
                speed=D_800966EE[((speed/4)&4095)*2]*factor/4096;
                if(speed<0) speed=0;
                speed+=(short)delta;
                asm volatile("" : : "r"(delta), "r"(speed));
                if(((short)angle-(state->yaw>>8))&2048) speed=-speed;
                state->turn=((int)((unsigned int)speed<<8))/128;
            } else state->turn=0;
            state->initialized=1;
        }
        {
            int cosine,sine,i;
            Matrix *matrix;
            register int *out asm("$6");
            Instance *source;
            state->yaw=(unsigned int)state->yaw+state->turn;
            cosine=rcos(state->yaw>>8);
            sine=rsin(state->yaw>>8);
            i=0; matrix=&D_8019A808; out=matrix->position; source=instance;
            matrix->rotation[0][0]=cosine;
            asm volatile("" : : : "memory");
            D_8019A80C=sine; D_8019A814=0u-sine; D_8019A818=cosine;
            D_8019A824=0; D_8019A820=0; D_8019A81C=0;
            D_8019A816=0; D_8019A812=0; D_8019A80E=0; D_8019A80A=0; D_8019A810=4096;
            for(;i<3;i++) { *out++=source->position[0]; source=(Instance *)((char *)source+4); }
        }
        if(state->brightness<64) return 0;
        {
            Matrix *matrix=&D_8019A808;
            Vector *in;
            register Vector *out asm("$4");
            int i;
            asm volatile("" : "=r"(matrix) : "0"(matrix) : "$2", "$3", "$4", "$5", "$6", "$7", "$8");
            gte_ldrotmatrix(matrix);
            gte_ldtransmatrix(matrix);
            i=0; out=local.collision.vertices; in=D_8019A574;
            for(;i<4;i++) {
                register int x asm("$12");
                register int y asm("$13");
                register int z asm("$14");
                gte_ldv0(in);
                gte_rtv0tr_mac();
                gte_mfc2_9(x); gte_mfc2_10(y); gte_mfc2_11(z);
                out->x=x; out->y=y; out->z=z;
                out++; in++;
            }
        }
        {
            int i,previous,point;
            register Vector *out asm("$6");
            int *area;
            i=0; area=&local.collision.area; out=local.collision.vertices;
            previous=((unsigned int)local.collision.vertices[3].z<<16)|(unsigned short)local.collision.vertices[3].x;
            point=((unsigned int)g_PlayerEntity->position[2]<<16)|(unsigned short)g_PlayerEntity->position[0];
            for(;i<4;i++) {
                int current=((unsigned int)out->z<<16)|(unsigned short)out->x;
                gte_ldsxy0(point); gte_ldsxy2(previous); gte_ldsxy1(current);
                gte_nclip(); gte_stmac0(area);
                if(local.collision.area<0) break;
                out++; previous=current;
            }
            if(i>=4) {
                unsigned char *inside=&D_8019A82F;
                if(!*inside) {
                    Owner *owner;
                    g_PlayerEntity->owner->status|=0x4000;
                    owner=instance->owner;
                    if(owner) owner->flags|=0x80000000;
                    *inside=1; D_8019A830=1;
                }
            } else D_8019A82F=0;
        }
    }
    goto done;
draw:
    {
        int page,palette;
        int texture;
        int rawPage;
        int kind;
        int specialKind;
        register int paletteOffset asm("$2");
        int i;
        int *in,*out;
        Matrix *transforms;
        i=0;
        base=D_8019A808;
        transforms=instance->transforms;
        out=base.position; in=transforms[33].position;
        for(;i<3;i++) *out++=*in++;
        rawPage=GetTPage(0,1,0,0);
        texture=D_800E2850[D_800E11FA];
        asm volatile("" : "=r"(texture) : "0"(texture), "r"(rawPage) : "memory");
        kind=D_800F336C;
        asm volatile("" : "=r"(kind) : "0"(kind), "r"(rawPage), "r"(texture));
        texture|=rawPage;
        page=(unsigned short)texture;
        asm volatile("" : "=r"(page) : "0"(page));
        paletteOffset=kind*2;
        specialKind=4;
        asm volatile("" : "=r"(specialKind) : "0"(specialKind), "r"(paletteOffset));
        palette=*(unsigned short *)((char *)D_800E1204+paletteOffset);
        if(kind==specialKind && D_800F3428) palette+=7; else palette+=3;
        palette=GetClut(0,palette);
        GsSetOrign(page,(unsigned short)palette);
        func_800C6ED8(1);
        func_800C6EF8(state->asset);
        func_800C6FA0(state->asset,(unsigned short)state->brightness);
        i=0;
        do {
            register int size asm("$17");
            register int next asm("$7");
            register int product asm("$3");
            register int high asm("$9");
            int intermediate;
            register void *clearTarget asm("$4");
            register int clearValue asm("$5");
            register int clearLength asm("$6");
            local.matrix=base;
            next=i+1;
            product=state->size*next;
            intermediate=product/3+(product>>31);
            asm("" : "=h"(intermediate) : "0"(intermediate));
            clearTarget=&scale;
            clearValue=0;
            clearLength=16;
            i=next;
            asm volatile("" : : "r"(clearTarget), "r"(clearValue), "r"(clearLength), "r"(i));
            product>>=31;
            asm("" : "=r"(high) : "0"(intermediate), "r"(product));
            size=high-product;
            memset(clearTarget,clearValue,clearLength);
            scale.x=size; scale.y=size; scale.z=2048;
            Gte_ScaleMatrix(&local.matrix,&scale);
            func_800C71E4(state->asset,&local.matrix);
        } while(i<3);
        func_800C6F4C(state->asset);
        i=0;
        position.x=instance->position[0]; position.y=instance->position[1]; position.z=instance->position[2];
        {
            int yaw;
            rotation.x=0;
            yaw=state->yaw;
            rotation.z=0;
            rotation.y=(yaw>>8)+2024;
        }
        {
            register int triple asm("$2")=state->size*3;
            register int size asm("$17");
            asm("" : "=r"(triple) : "0"(triple));
            size=triple*2;
            do {
                func_800D0E88(&position,&rotation,size,512,D_8019A594,D_8019A3C8,D_8019A3C8,state->brightness,1);
                rotation.y=(unsigned short)rotation.y+24;
            } while(++i<3);
        }
    }
done:
    return 0;
}
