/* Matching candidate for room_m273, file offset 0x79F0, 1364 bytes.
 * Not a configured production source. See README.md for verification and debt.
 */
#include "pe1/gte.h"
typedef struct { short x,y,z,w; } Vector;
typedef struct { short m[3][3]; int t[3]; } Matrix;
typedef struct { Vector position,rotation,velocity; } Particle;
typedef struct { unsigned char unknown[8]; void *pool; } Context;
typedef struct { unsigned char unknown[0x2A]; short x,unknown2C,y,unknown30,z; } Player;
typedef struct { unsigned char unknown[8]; void *value; } Owner;
typedef struct { Owner *owner; } State;
typedef struct { unsigned char unknown[8]; State *state; } StateContext;
extern Context *D_800F33E0;
extern StateContext *D_800F32D0;
extern Player *g_PlayerEntity;
extern unsigned char D_8019AF69,D_8019AF68;
extern short D_8019AEFC,D_8019AEFE,D_8019AF00;
extern unsigned short D_8019AF60;
extern Vector D_8019AD60;
extern int D_800966EC[];
extern unsigned short D_800E11E8,D_800E2850[];
extern unsigned short D_800F3368,D_800F336A,D_800F336C,D_800F336E;
extern unsigned short D_800F3370,D_800F3372,D_800F3374;
extern volatile unsigned short D_800F3376,D_800F3378;
extern int func_8019665C();
extern int func_800CE560(void *,int,int,int (*)());
extern Particle *func_800CE610(void *);
extern int func_8005186C(int),func_80079FB4(int,int);
extern void func_80079754(Vector *,Matrix *);
extern void func_8006DCE4(int,void *,int,int,int);

int func_801969D8(int mode) {
    Vector playerPosition;
    Matrix matrix;
    switch(mode) {
    case 0:
        return func_800CE560(D_800F33E0->pool,24,40,func_8019665C);
    case 1: {
        register unsigned char *stopped asm("$17")=&D_8019AF69;
        Player *player;
        int dx,dy,dz,x,y,z; short spread; short angle;
        if(*stopped) return 2;
        player=g_PlayerEntity;
        playerPosition.x=x=player->x;
        dx=x-D_8019AEFC;
        playerPosition.y=y=player->y;
        playerPosition.z=z=player->z;
        dz=z-D_8019AF00;
        dy=y-D_8019AEFE;
        angle=func_80079FB4(dy,func_8005186C((unsigned int)dx*dx+(unsigned int)dz*dz))&4095;
        if(angle>768) angle=768;
        if(angle<384) angle=384;
        /* Pitch is clamped to [384, 768]; the second ring halves its spread. */
        spread=angle-256;
        if(D_8019AF68) {
            short i=0;
            int width=(short)spread;
            /* Retail address window: heading 0x8019AF60, origin 0x8019AEFC. */
            register unsigned short *heading asm("$19")=(unsigned short *)(stopped-9);
            register Matrix *transform asm("$16")=&matrix;
            matrix.t[0]=0; matrix.t[1]=0; matrix.t[2]=0;
            do {
                Particle *particle=func_800CE610(D_800F33E0->pool);
                short *wave;
                if(!particle) break;
                wave=(short *)&D_800966EC[(i*4096/6)&4095];
                particle->rotation.x=angle+wave[1]*160/4096;
                particle->rotation.y=wave[0]*width/4096+(*heading+2048);
                particle->rotation.z=0; particle->rotation.w=1;
                func_80079754(&particle->rotation,transform);
                gte_ldrotmatrix(transform); gte_ldtransmatrix(transform);
                { Vector *input=&D_8019AD60; gte_ldv0(input); }
                gte_rtv0tr_mac();
                {
                    register Vector *out=&particle->velocity;
                    register int x asm("$12"),y asm("$13"),z asm("$14");
                    /* Keep the C stores relative to the GTE output pointer. */
                    asm("" : "=r"(out) : "0"(out));
                    gte_mfc2_9(x); gte_mfc2_10(y); gte_mfc2_11(z);
                    out->x=x; out->y=y; out->z=z;
                }
                particle->position.x=heading[-50];
                particle->position.y=heading[-49];
                particle->position.z=heading[-48];
                particle->position.w=0; particle->velocity.w=0;
                i++;
            } while(i<6);
            /* Failure in the six-particle ring does not skip this ring. */
            i=0; width=(short)spread/2; heading=&D_8019AF60;
            do {
                Particle *particle=func_800CE610(D_800F33E0->pool);
                short *wave;
                if(!particle) break;
                {
                    register int *table asm("$3")=D_800966EC;
                    wave=(short *)&table[((unsigned int)i<<10)&3072];
                }
                particle->rotation.x=angle+wave[1]*96/4096;
                particle->rotation.y=wave[0]*width/4096+(*heading+2048);
                particle->rotation.z=0; particle->rotation.w=1;
                asm("" : : "r"(&particle->rotation));
                transform=&matrix;
                func_80079754(&particle->rotation,transform);
                gte_ldrotmatrix(transform); gte_ldtransmatrix(transform);
                { Vector *input=&D_8019AD60; gte_ldv0(input); }
                gte_rtv0tr_mac();
                {
                    register Vector *out=&particle->velocity;
                    register int x asm("$12"),y asm("$13"),z asm("$14");
                    /* Keep the C stores relative to the GTE output pointer. */
                    asm("" : "=r"(out) : "0"(out));
                    gte_mfc2_9(x); gte_mfc2_10(y); gte_mfc2_11(z);
                    out->x=x; out->y=y; out->z=z;
                }
                particle->position.x=heading[-50];
                particle->position.y=heading[-49];
                particle->position.z=heading[-48];
                particle->position.w=0; particle->velocity.w=0;
                i++;
            } while(i<4);
            func_8006DCE4(0x5D1,D_800F32D0->state->owner->value,D_8019AEFC,D_8019AEFE,D_8019AF00);
        }
        break;
    }
    case 2: {
        unsigned int index=D_800E11E8;
        unsigned short palette;
        D_800F3368=16; D_800F336A=1;
        D_800F3376=16; D_800F3378=16;
        D_800F3376=16; D_800F3378=16;
        palette=D_800E2850[index];
        D_800F336C=2; D_800F336E=0; D_800F3372=0; D_800F3374=0;
        D_800F3370=palette;
        break;
    }
    }
    return 0;
}
