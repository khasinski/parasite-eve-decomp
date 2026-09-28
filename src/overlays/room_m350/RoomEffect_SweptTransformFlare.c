#include "pe1/room_m350_effects.h"
#include "pe1/gte.h"


extern RoomM350EffectActor *D_800F32D0;
extern GteMatrix D_8019A870;
extern short D_8019A890[];
extern int D_800E27EC, D_800966EC[], D_800F3428, D_8019A690[];
extern unsigned short D_800F336C, D_800E1204[];
extern short D_800F336A;
extern int GetClut(int, int);
extern void func_800CEE20(void *, void *, int, int, int, int, int, int, void *);

int func_801981D0(int event, RoomM350FlareParticle *particle)
{
    GteShortVector position, rotation;
    if (event == 1) {
        int timer = D_800E27EC;
        if (timer >= 8) return 1;
        particle->shade = (short)D_800966EC[((unsigned int)timer << 8) & 0xF00] >> 5;
    } else if (event == 2) {
        int kind, palette;
        unsigned short clut;
        position.x = 0;
        position.y = particle->y;
        position.z = particle->z;
        {
            register GteMatrix *matrix asm("$8") = &D_8019A870;
            GteShortVector *out;
            gte_ldrotmatrix(matrix);
            gte_ldtransmatrix(matrix);
            out = &position;
            gte_ldv0(out);
            gte_rtv0tr_mac();
            asm volatile("" : "=r"(out) : "0"(out));
            {
                register int x asm("$12");
                register int y asm("$13");
                register int z asm("$14");
                gte_mfc2_9(x); gte_mfc2_10(y); gte_mfc2_11(z);
                out->x = x; out->y = y; out->z = z;
            }
        }
        position.x += D_8019A890[0];
        position.y += D_8019A890[1];
        position.z += D_8019A890[2];
        rotation.x = (unsigned int)D_800E27EC * 384;
        rotation.y = D_800F32D0->instance->yaw;
        rotation.z = 0;
        rotation.pad = 1;
        kind = D_800F336C;
        palette = D_800E1204[kind];
        if (kind == 4 && D_800F3428) palette += 4;
        clut = GetClut(0, palette);
        {
            int timer = D_800E27EC;
            register int scale asm("$4") = D_800F336A;
            register int phase asm("$3") = timer & 7;
            register int product asm("$8") = scale * phase;
            int texture = product + 64;
            asm("" : : "r"(product), "r"(texture));
            func_800CEE20(&position, &rotation, 0x2200, 4096,
                texture, clut, 1, particle->shade, &D_8019A690[timer & 3]);
        }
    }
    return 0;
}


/* Spawns flares and tests the quad swept by the actor's two side endpoints. */
extern RoomM350EffectEmitter *D_800F33E0;
extern RoomM350EffectInstance *g_PlayerEntity;

extern unsigned char D_8019A8C0;
extern short D_8019A898[4],D_8019A8A0[4];
/* Separate field-store views preserve reloads of the complete records. */
extern volatile short rightX asm("D_8019A898");
extern volatile short leftX asm("D_8019A8A0");
extern GteShortVector D_8019A8A8,D_8019A8B0;
extern volatile short D_8019A89C,D_8019A89E,D_8019A8A4;
extern short D_8019A8AE;
extern short D_800F3368,D_800F336A,D_800F3376,D_800F3378;
extern short D_800F336E,D_800F3372,D_800F3374,D_800F3370;
extern unsigned short D_800E11EA,D_800E2850[];
extern int func_800CE560(void *,int,int,int (*)(int,RoomM350FlareParticle *));
extern RoomM350FlareParticle *func_800CE610(void *);
extern int Inv_ScrambleGrid(void);

int func_80198400(int event) {
    GteShortVector offset,vertices[4];
    int area;
    RoomM350EffectInstance *instance=D_800F32D0->instance;
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
        RoomM350FlareParticle *particle=func_800CE610(D_800F33E0->pool);
        if(particle) {
            int random=Inv_ScrambleGrid()&255;
            particle->y=random;
            particle->z=(random>>2)+64;
        }
    }
    if(D_8019A8C0 || instance->animation!=9) return 0;
    {
        register GteShortVector *from asm("$5")=(GteShortVector *)&D_8019A898;
        register GteShortVector *to asm("$4")=&D_8019A8A8;
        asm("" : "=r"(from),"=r"(to) : "0"(from),"1"(to));
        *to=*from;
    }
    {
        register GteShortVector *from asm("$5")=(GteShortVector *)&D_8019A8A0;
        register GteShortVector *to asm("$4")=&D_8019A8B0;
        asm("" : "=r"(from),"=r"(to) : "0"(from),"1"(to));
        *to=*from;
    }
    offset.x=256; offset.y=0; offset.z=0;
    ApplyMatrixSV(&instance->transform,&offset,&offset);
    {
        int positionX=instance->transform.t[0];
        unsigned short x;
        unsigned short z;
        int positionZ;
        x=offset.x;
        z=offset.z;
        rightX=x+positionX;
        positionZ=((volatile RoomM350EffectInstance *)instance)->transform.t[2];
        D_8019A89E=1;
        positionZ=z+positionZ;
        D_8019A89C=positionZ;
        leftX=((volatile RoomM350EffectInstance *)instance)->transform.t[0]-x;
        D_8019A8A4=((volatile RoomM350EffectInstance *)instance)->transform.t[2]-z;
    }
    asm volatile("" ::: "memory");
    vertices[0]=*(GteShortVector *)&D_8019A898;
    vertices[1]=*(GteShortVector *)&D_8019A8A0;
    vertices[2]=D_8019A8B0;
    {
        int valid=D_8019A8AE;
        vertices[3]=D_8019A8A8;
        if(!valid) return 0;
        {
            int i=0;
            int previous=((unsigned int)vertices[3].z<<16)|(unsigned short)vertices[3].x;
            int point=((unsigned int)g_PlayerEntity->transform.t[2]<<16)|(unsigned short)g_PlayerEntity->transform.t[0];
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
                RoomM350EffectInstance *player=g_PlayerEntity;
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
        GteMatrix *transforms;
        transforms=instance->transforms;
        for(;i<3;i++) destination[i]=(transforms[11].t[i]+transforms[15].t[i])>>1;
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
